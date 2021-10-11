#ifndef _VOXOMAP_ARRAYSUPERCONTAINER_HPP_
#define _VOXOMAP_ARRAYSUPERCONTAINER_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <functional>
#include "../iterator.hpp"

namespace voxomap
{

template <class T>
class PoolAllocator
{
public:
    static_assert(sizeof(T) >= sizeof(void*), "Impossible pool allocation");

    template <typename... Args>
    static T* get(Args&&... args)
    {
        if (_available == nullptr)
        {
            return new T(std::forward<Args>(args)...);
        }
        else
        {
            T* elem = reinterpret_cast<T*>(_available);
            _available = _available->next;
            new (elem) T(std::forward<Args>(args)...);
            return elem;
        }
    }

    static void release(T* ptr)
    {
        ptr->~T();
        Node* node = reinterpret_cast<Node*>(ptr);
        node->next = _available;
        _available = node;
    }

    static void reserve(size_t nb)
    {
        //auto* prev = nullptr;
        for (size_t i = 0; i < nb; ++i)
        {
            auto* tmp = reinterpret_cast<Node*>(operator new(sizeof(T)));
            tmp->next = _available;
            _available = tmp;
        }
    }

private:
    struct Node
    {
        Node* next;
    };

    static Node* _available;
};

template <class T>
typename PoolAllocator<T>::Node* PoolAllocator<T>::_available = nullptr;

template <typename T>
class DefaultPoolAllocator
{
public:
    template <typename... Args>
    static T* get(Args&&... args)
    {
        return new T(std::forward<Args>(args)...);
    }

    static void release(T* ptr)
    {
        delete ptr;
    }

    static void reserve(size_t nb)
    {
    }
};

/*!
    \defgroup SuperContainer SuperContainer
    Super container used in leaves of the VoxelOctree to store others super or voxel containers.
    \ingroup VoxelOctree
*/

template <class Container> class VoxelNode;

/*! \struct ArraySuperContainer
    \ingroup SuperContainer
    \brief Super container used in leaves of the VoxelOctree.
    Based on a fixed size 3D array, useful for high density of voxels.
    - Advantage:
        - Good global performances
    - Disadvantage:
        - Big memory footprint (RAM and serialized), same footprint with 1 or 512 voxel/super sub-container.
*/
template <class T_Container, class T_PoolAllocator = DefaultPoolAllocator<T_Container>>
struct ArraySuperContainer
{
    using Container = T_Container;
    using VoxelData = typename Container::VoxelData;
    using VoxelContainer = typename Container::VoxelContainer;
    using iterator = supercontainer_iterator<ArraySuperContainer<Container>>;

    const static uint32_t NB_CONTAINERS = 8;
    const static uint32_t CONTAINER_MASK = NB_CONTAINERS - 1;
    const static uint32_t NB_VOXELS = NB_CONTAINERS * Container::NB_VOXELS;
    const static uint32_t COORD_MASK = ~(NB_VOXELS - 1);
    const static uint32_t VOXEL_MASK = Container::VOXEL_MASK;
    const static uint32_t NB_SUPERCONTAINER = 1 + Container::NB_SUPERCONTAINER;
    const static uint32_t SUPERCONTAINER_ID = NB_SUPERCONTAINER - 1;

    /*!
        \brief Default constructor
    */
    ArraySuperContainer();
    /*!
        \brief Copy constructor
    */
    ArraySuperContainer(ArraySuperContainer const& other);
    /*!
        \brief Default move constructor
    */
    ArraySuperContainer(ArraySuperContainer&& other) = default;
    /*!
        \brief Default destructor
    */
    ~ArraySuperContainer();


    /*!
        \brief Initialization method, do nothing
    */
    void                init(VoxelNode<ArraySuperContainer<Container>> const&) {}
    /*!
        \brief Returns number of voxels
    */
    uint32_t            getNbVoxel() const;
    /*!
        \brief Find voxel
        \param it Iterator containing voxel position information
        \return The voxel if exists, otherwise nullptr
    */
    template <typename Iterator>
    VoxelData*          findVoxel(Iterator& it);
    /*!
        \brief Find voxel
        \param it Iterator containing voxel position information
        \return The voxel if exists, otherwise nullptr
    */
    template <typename Iterator>
    VoxelData const*    findVoxel(Iterator& it) const;

    /*!
        \brief Check if there is sub-container
        \param x X index
        \return True if there is sub-container
    */
    bool                hasContainer(uint8_t x) const;
    /*!
        \brief Check if there is sub-container
        \param x X index
        \param y Y index
        \return True if there is sub-container
    */
    bool                hasContainer(uint8_t x, uint8_t y) const;
    /*!
        \brief Check if there is sub-container
        \param x X index
        \param y Y index
        \param z Z index
        \return True if there is sub-container
    */
    bool                hasContainer(uint8_t x, uint8_t y, uint8_t z) const;
    /*!
        \brief Find sub-container
        \param x X index
        \param y Y index
        \param z Z index
        \return Pointer on the sub-container if exists, otherwise nullptr
    */
    Container*          findContainer(uint8_t x, uint8_t y, uint8_t z);
    /*!
        \brief Find sub-container
        \param x X index
        \param y Y index
        \param z Z index
        \return Pointer on the sub-container if exists, otherwise nullptr
    */
    Container const*    findContainer(uint8_t x, uint8_t y, uint8_t z) const;

    /*!
        \brief Add a voxel, don't update an existing voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
        \return True if success and update \a it
    */
    template <typename Iterator, typename... Args>
    int                 addVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Update an existing voxel, don't create a new one
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
        \return True if success
    */
    template <typename Iterator, typename... Args>
    int                 updateVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Add or update a voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
    */
    template <typename Iterator, typename... Args>
    int                 putVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Remove an existing voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to removeVoxel method of the container
        \return True if success
    */
    template <typename Iterator, typename... Args>
    int                 removeVoxel(Iterator const& it, Args&&... args);

    /*!
        \brief Serialize the structure
        \param str String use for save the serialization
    */
    void                serialize(std::string& str) const;
    /*!
        \brief Unserialize \a str inside \a this
        \param str String that contains data
        \param size Size of the string
        \return Number of bytes read inside str
    */
    size_t              unserialize(char const* str, size_t size);

    /*!
        \brief Go through all voxels of the container and call the \a predicate for each
        \param it Begin iterator
        \param predicate Function called for each voxel found
    */
    template <typename Iterator>
    void                exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const;

    void                exploreVoxelContainer(std::function<void(typename Container::VoxelContainer const&)> const& predicate) const;

private:
    Container* _containerArray[NB_CONTAINERS][NB_CONTAINERS][NB_CONTAINERS] = { nullptr };  //!< Array of voxel containers
    uint32_t _nbVoxels = 0; //!< Number of voxels
};

}

#include "ArraySuperContainer.ipp"

#endif // _VOXOMAP_ARRAYSUPERCONTAINER_HPP_
