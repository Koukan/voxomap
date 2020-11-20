#ifndef _VOXOMAP_SIDEDCONTAINER_HPP_
#define _VOXOMAP_SIDEDCONTAINER_HPP_

#include <cstdint>
#include <utility>
#include <type_traits>
#include "../VoxelNode.hpp"
#include "../iterator.hpp"

namespace voxomap
{

/*!
    \addtogroup SidedContainer SidedContainer
    \ingroup VoxelContainer
    Classes uses for side features
*/

/*! \enum SideEnum
    \ingroup SidedContainer
    \brief List of the voxel sides
*/
enum SideEnum : uint8_t
{
    XPOS = 1,       //!< x + 1 of the voxel
    XNEG = 2,       //!< x - 1 of the voxel
    YPOS = 4,       //!< y + 1 of the voxel
    YNEG = 8,       //!< y - 1 of the voxel
    ZPOS = 16,      //!< z + 1 of the voxel
    ZNEG = 32,      //!< z - 1 of the voxel
    ALL = XPOS | XNEG | YPOS | YNEG | ZPOS | ZNEG //!< All side enum
};

/*! \struct SidedVoxel
    \ingroup SidedContainer
    \brief Voxel with side informations
    Side is present if the corresponding neighbor voxel exist.
    For example:
    - If YPOS is present, a voxel is present at y + 1
    - If XNEG is present, a voxel is present at x - 1
*/
template <class T_Voxel>
struct SidedVoxel : T_Voxel
{
    /*!
        \brief Default constructor
    */
    SidedVoxel() = default;
    /*!
        \brief Constructor with one argument forward to Voxel constructor
    */
    template <typename T, typename = typename std::enable_if<!std::is_class<T>::value, int>::type>
    SidedVoxel(T arg);
    /*!
        \brief Constructor with one argument forward to Voxel constructor
    */
    template <typename T, typename = typename std::enable_if<std::is_class<T>::value, int>::type>
    SidedVoxel(T const& arg);
    /*!
        \brief Constructor with arguments forward to Voxel constructor
    */
    template <typename T, typename U, typename... Args>
    SidedVoxel(T&& arg_1, U&& arg_2, Args&&... args);
    /*!
        \brief Copy constructor
    */
    SidedVoxel(SidedVoxel const& other) = default;
    /*!
        \brief Move constructor
    */
    SidedVoxel(SidedVoxel&& other) = default;
    /*!
        \brief Constructor with side information
        \param side Side information
        \param args Arguments forward to voxel constructor
    */
    template <typename... Args>
    SidedVoxel(SideEnum side, Args&&... args);

    /*!
        \brief Default assignement operator
    */
    SidedVoxel& operator=(SidedVoxel const& other) = default;
    /*!
        \brief Default move assignement operator
    */
    SidedVoxel& operator=(SidedVoxel&& other) = default;

    operator    bool() const;
    /*!
        \brief Call the T_Voxel method operator==
    */
    bool        operator==(SidedVoxel const& other) const;
    /*!
        \brief Call the T_Voxel method operator!=
    */
    bool        operator!=(SidedVoxel const& other) const;
    /*!
        \brief Call the T_Voxel method operator==
    */
    bool        operator==(T_Voxel const& other) const;
    /*!
        \brief Call the T_Voxel method operator!=
    */
    bool        operator!=(T_Voxel const& other) const;
    /*!
        \brief True if the side of \a this is equal to \a s
    */
    bool        operator==(uint8_t s) const;
    /*!
        \brief True if the side of \a this is different to \a s
    */
    bool        operator!=(uint8_t s) const;
    /*!
        \brief Returns AND of internal side with s
    */
    uint8_t     operator&(uint8_t s) const;
    /*!
        \brief Returns OR of internal side with s
    */
    uint8_t     operator|(uint8_t s) const;
    /*!
        \brief Returns AND of internal side with s
    */
    uint8_t     operator&(SideEnum s) const;
    /*!
        \brief Returns OR of internal side with s
    */
    uint8_t     operator|(SideEnum s) const;
    /*!
        \brief Apply AND
        \return Reference to \a this
    */
    SidedVoxel& operator&=(uint8_t s);
    /*!
        \brief Apply OR
        \return Reference to \a this
    */
    SidedVoxel& operator|=(uint8_t s);
    /*!
        \brief Returns internal side
    */
    uint8_t     getSide() const;

private:
    uint8_t _voxel_side = 0;
};

/*! \struct SidedContainer
    \ingroup SidedContainer
    \brief Wrap a voxel container to add side features

*/
template <template <class...> class T_Container, class T_Voxel>
struct SidedContainer : public T_Container<SidedVoxel<T_Voxel>>
{
    using VoxelData = SidedVoxel<T_Voxel>;
    using VoxelContainer = SidedContainer<T_Container, T_Voxel>;
    using iterator = container_iterator<SidedContainer<T_Container, T_Voxel>>;

    const static uint32_t NB_VOXELS = T_Container<VoxelData>::NB_VOXELS;
    const static uint32_t COORD_MASK = ~(NB_VOXELS - 1);
    const static uint32_t VOXEL_MASK = NB_VOXELS - 1;
    const static uint32_t NB_SUPERCONTAINER = 0;

    /*!
        \brief Default constructor
    */
    SidedContainer() = default;
    /*!
        \brief Default copy constructor
    */
    SidedContainer(SidedContainer const& other) = default;
    /*!
        \brief Default move constructor
    */
    SidedContainer(SidedContainer&& other) = default;

    /*!
        \brief Initialization method, do nothing
    */
    void                init(VoxelNode<SidedContainer> const&) {}
    /*!
        \brief Returns number of sides
    */
    inline uint16_t     getNbSide() const;

    /*!
        \brief Add a voxel, don't update an existing voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
        \return True if success and update \a it
    */
    template <typename Iterator, typename... Args>
    bool                addVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Update an existing voxel, don't create a new one
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
        \return True if success
    */
    template <typename Iterator, typename... Args>
    bool                updateVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Add or update a voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
    */
    template <typename Iterator, typename... Args>
    void                putVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Remove an existing voxel
        \param it Iterator that contains the informations
        \param voxel Pointer on a voxel structure, filled with the data of the removed voxel
        \return True if success
    */
    template <typename Iterator>
    bool                removeVoxel(Iterator const& it, VoxelData* voxel = nullptr);

    void                exploreVoxelContainer(std::function<void(SidedContainer const&)> const& predicate) const;

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

private:
    // Side management
    template <typename Iterator>
    void addSide(Iterator const& it);
    template <typename Iterator>
    void removeSide(Iterator const& it);
    template <typename Iterator>
    void updateSide(Iterator const& it);
    template <class Iterator> friend void addSide(Iterator const& otherIt, SideEnum side);
    template <class Iterator> friend void removeSide(Iterator const& currentIt, Iterator const& otherIt, SideEnum s1, SideEnum s2);
    template <class Iterator> friend void updateSide(Iterator const& currentIt, Iterator const& otherIt, SideEnum s1, SideEnum s2);

    uint16_t _nbSides = 0;
};

}

#include "SidedContainer.ipp"

#endif // _VOXOMAP_SidedContainer_HPP_
