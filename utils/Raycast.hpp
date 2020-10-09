#ifndef _VOXOMAP_RAYCAST_HPP_
#define _VOXOMAP_RAYCAST_HPP_

#include <functional>
#include <unordered_map>
#include <map>
#include <bitset>
#include <type_traits>
#include <memory>
#include "Ray.hpp"
#include "../voxel_octree/VoxelContainer/SidedContainer.hpp"
#include "../voxel_octree/VoxelOctree.hpp"

namespace voxomap
{

/*! \class Raycast
    \ingroup Utility
    \brief Utility class to do ray casting inside a VoxelOctree.
*/
template <class T_Container>
class Raycast
{
public:
    using VoxelData = typename T_Container::VoxelData;
    using T_Node = VoxelNode<T_Container>;
    using T_Octree = VoxelOctree<T_Container>;
    using T_VoxelContainer = typename T_Container::VoxelContainer;
    using iterator = typename T_Container::iterator;
    using Predicate = std::function<bool(iterator const&)>;

    /*! \struct Result
        \brief Result of a ray cast
    */
    struct Result
    {
        /*!
            \brief Compare two result by the distance
            \return true if \a this is closer than \a other
        */
        bool    operator<(Result const& other) const;
        /*!
            \brief Compare two result by the distance
            \return true if \a this is further than \a other
        */
        bool    operator>(Result const& other) const;

        Vector3D            position;               //!< Position of the intersection point
        double              distance = -1;          //!< Squared distance between the source point and the intersection point
        SideEnum            side = SideEnum::YPOS;  //!< Side intersected
        iterator            it;
    };

    /*! \struct Cache
        \brief Structure used for improve performance of raycasting
    */
    struct Cache
    {
        Cache();
        Cache(Cache const& other);

        inline bool empty() const { return _nodeCache->empty(); }

        /*!
            \brief Check if \a node has voxel inside the box, don't fill the cache
            \param node The node
            \param boxPosition Position of the box
            \param boxSize Size of the box
            \return True if there is voxel
        */
        bool hasVoxel(T_Node const& node, Vector3I const& boxPosition, int boxSize);
        /*!
            \brief Check if \a node has voxel inside the box and fill the cache if it is not already filled
            \param node The node
            \param boxPosition Position of the box
            \param boxSize Size of the box
            \return True if there is voxel
        */
        bool fillHasVoxel(T_Node const& node, Vector3I const& boxPosition, int boxSize);
        /*!
            \brief Initialize the cache with \a node input
            \param node The node
        */
        void fillCache(T_Node const& node);
        /*!
            \brief Initialize the cache
            \param octree The octree
        */
        void fillCache(T_Octree const& octree);

    private:
        /*! \struct BoxPresenceCache
            \brief A cache structure that store presence of voxel/container inside a container
        */
        struct PresenceCache
        {
            bool hasVoxel(Cache& cache, Vector3I const& boxPosition, int boxSize) const;
            inline bool hasVoxel() const { return reinterpret_cast<uint64_t const*>(presence) != 0; }

            uint8_t presence[8] = { 0 }; //!< Represent the presence of voxel/container inside the node

            static_assert(sizeof(PresenceCache::presence) == sizeof(uint64_t), "PresenceCache hasVoxel method won't work.");
        };

        /*! \struct ContainerPresenceCache
            \brief A cache structure that store presence of VoxelContainer inside a SuperContainer
        */
        template <typename T_SubContainer>
        struct ContainerPresenceCache : PresenceCache
        {
            bool hasVoxel(Cache& cache, Vector3I const& boxPosition, int boxSize) const;

            const static uint32_t NB_CONTAINERS = T_SubContainer::NB_CONTAINERS;
            PresenceCache containerPresence[NB_CONTAINERS][NB_CONTAINERS][NB_CONTAINERS];
        };

        /*! \struct SuperContainerPresenceCache
            \brief A cache structure that store presence of SuperContainer inside a SuperContainer
        */
        template <typename T_SubContainer>
        struct SuperContainerPresenceCache : PresenceCache
        {
            using SubCache = typename std::conditional<(T_SubContainer::NB_SUPERCONTAINER > 2),
                SuperContainerPresenceCache<typename T_SubContainer::Container>,
                ContainerPresenceCache<typename T_SubContainer::Container>>::type;

            bool hasVoxel(Cache& cache, Vector3I const& boxPosition, int boxSize) const;

            const static uint32_t NB_CONTAINERS = T_SubContainer::NB_CONTAINERS;
            std::unique_ptr<SubCache> containerPresence[NB_CONTAINERS][NB_CONTAINERS][NB_CONTAINERS];
        };

        /* NodeCache
        * if (T_Container::NB_SUPERCONTAINER > 1)
        *   NodeCache = SuperContainerPresenceCache<T_Container>
        * else if (T_Container::NB_SUPERCONTAINER == 1)
        *   NodeCache = ContainerPresenceCache<T_Container>
        * else
        *   NodeCache = PresenceCache
        */
        using NodeCache = typename std::conditional<(T_Container::NB_SUPERCONTAINER > 1),
            SuperContainerPresenceCache<T_Container>,
            typename std::conditional<T_Container::NB_SUPERCONTAINER == 1,
                ContainerPresenceCache<T_Container>,
            PresenceCache>::type>::type;

        /*!
            \brief Initialize the \a cache for SuperContainerPresenceCache
            \param node Concerned node
            \param cache Cache structure
        */
        template <class T_SubContainer, typename T_Cache>
        void fillCache(T_SubContainer const& container, T_Cache& cache);
        /*!
            \brief Initialize the \a cache for ContainerPresenceCache
            \param node Concerned node
            \param cache Cache structure
        */
        template <class T_SubContainer>
        void fillCache(T_SubContainer const& container, ContainerPresenceCache<T_SubContainer>& cache);
        /*!
            \brief Initialize the \a cache for PresenceCache
            \param node Concerned node
            \param cache Cache structure
        */
        template <class T_SubContainer>
        void fillCache(T_SubContainer const& container, PresenceCache& cache);

        std::shared_ptr<std::unordered_map<T_Node const*, NodeCache>> _nodeCache; //!< Cache memory
        T_Node const* _lastNode = nullptr;
        NodeCache* _lastCache = nullptr;
    };

    /*!
        \brief Execute a ray cast on the \a node
        \param node The node where to execute the ray cast
        \return True if there is an intersection
    */
    bool                execute(T_Node const& node);

    /*!
        \brief Execute a raycast
        \param ray The ray to cast
        \param node Node where to execute the raycast
        \param predicate Function that allow to add some conditions in raytracing
        \param maxDistance Maximum distance where to execute the ray casting
        \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Node const& node, Predicate const& predicate, double maxDistance = -1);
    /*!
        \brief Execute a raycast
        \param ray The ray to cast
        \param node Node where to execute the raycast
        \param maxDistance Maximum distance where to execute the ray casting
        \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Node const& node, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param predicate Function that allow to add some conditions in raytracing
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Octree const& octree, Predicate const& predicate, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Octree const& octree, double maxDistance = -1);

    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param node Node where to execute the raycast
       \param cache The cache structure
       \param predicate Function that allow to add some conditions in raytracing
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Node const& node, Cache& cache, Predicate const& predicate, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param node Node where to execute the raycast
       \param cache The cache structure
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Node const& node, Cache& cache, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param cache The cache structure
       \param predicate Function that allow to add some conditions in raytracing
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Octree const& octree, Cache& cache, Predicate const& predicate, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param cache The cache structure
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Octree const& octree, Cache& cache, double maxDistance = -1);

    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param nodes Array of nodes where to execute the raycast
       \param nbNode Number of nodes inside the array
       \param cache The cache structure
       \param predicate Function that allow to add some conditions in raytracing
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, T_Node const* const* nodes, size_t nbNode, Cache& cache, Predicate const& predicate, double maxDistance = -1);

    Ray         ray;                //!< The ray to cast
    double      maxDistance = -1;   //!< The maximum distance of the ray cast
    Predicate   predicate;          //!< Function that allow to add some conditions in raytracing
    Result      result;             //!< The ray cast result

private:
    /*!
        \brief Raycast on a voxel
        \param node Node where the voxel is
        \param pos Position of the voxel
        \return True if ray intersect the voxel
    */
    bool raycastVoxel(iterator& it, T_VoxelContainer const& container);
    /*!
        \brief Raycast on a box
        \param node Node where the box is
        \param boxPosition Position of the box
        \param boxSize Size of the box
        \return True if ray intersect a voxel inside the box
    */
    bool raycastContainer(iterator& it, T_VoxelContainer const& container, Vector3I const& boxPosition, int boxSize);
    template <typename T>
    bool raycastContainer(iterator& it, T const& container, Vector3I const& boxPosition, int boxSize);
    /*!
        \brief Raycast on a node
        \param node The node
        \return True if ray intersect a voxel inside the node
    */
    bool raycast(T_Node const& node);

    int         _sortingIndex = 0; //!< Index inside hardcoded array, improve ray casting performance
    SideEnum    _sideToCheck = SideEnum::ALL; //!< Side to check with the ray cast>
    Cache       _cache; //!< Cache structure, to improve performance
};

}

#include "Raycast.ipp"

#endif // _RAYCAST_HPP_