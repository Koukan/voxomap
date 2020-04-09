#ifndef _VOXOMAP_RAYCAST_HPP_
#define _VOXOMAP_RAYCAST_HPP_

#include <functional>
#include <unordered_map>
#include <bitset>
#include "Ray.hpp"
#include "../voxel_octree/SideArea.hpp"
#include "../voxel_octree/VoxelOctree.hpp"

namespace voxomap
{

/*! \class Raycast
    \ingroup Utility
    \brief Utility class to do ray casting inside a VoxelOctree.
*/
template <class T_Area>
class Raycast
{
public:
    using VoxelData = typename T_Area::VoxelData;
    using Predicate = std::function<bool(VoxelNode<T_Area> const&, VoxelData const&, Vector3I const&)>;

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

        Vector3D                    position;               //!< Position of the intersection point
        Vector3D                    voxelPosition;          //!< Position of the intersected voxel
        double                      distance = -1;          //!< Squared distance between the source point and the intersection point
        VoxelNode<T_Area> const*    node = nullptr;         //!< Node where the intersected voxel is
        VoxelData const*            voxel = nullptr;        //!< The intersected voxel
        SideEnum                    side = SideEnum::TOP;   //!< Side intersected
    };

    /*! \struct Cache
        \brief Structure used for improve performance of raycasting
    */
    struct Cache
    {
        /*! \struct NodeCache
            \brief A cache structure that store presence of voxel inside a node
        */
        struct NodeCache
        {
            std::bitset<8>      hasVoxelIn4; //!< Represent the presence of voxel inside box of size 4 inside the node
            std::bitset<64>     hasVoxelIn2; //!< Represent the presence of voxel inside box of size 2 inside the node
        };

        /*!
            \brief Check if \a node has voxel inside the box, don't fill the cache
            \param node The node
            \param boxPosition Position of the box
            \param boxSize Size of the box
            \return True if there is voxel
        */
        bool hasVoxel(VoxelNode<T_Area> const& node, Vector3I const& boxPosition, size_t boxSize) const;
        /*!
            \brief Check if \a node has voxel inside the box and fill the cache if it is not already filled
            \param node The node
            \param boxPosition Position of the box
            \param boxSize Size of the box
            \return True if there is voxel
        */
        bool fillHasVoxel(VoxelNode<T_Area> const& node, Vector3I const& boxPosition, size_t boxSize);
        /*!
            \brief Initialize the cache with \a node input
            \param node The node
        */
        void fillCache(VoxelNode<T_Area> const& node);
        /*!
            \brief Initialize the cache
            \param octree The octree
        */
        void fillCache(VoxelOctree<T_Area> const& octree);

    private:
        /*!
            \brief Initialize the bitset inside \a cache
            \param node Concerned node
            \param cache Cache structure
        */
        void fillCache(VoxelNode<T_Area> const& node, NodeCache& cache);

        std::unordered_map<VoxelNode<T_Area> const*, NodeCache> _nodeCache; //!< Cache memory
    };

    /*!
        \brief Execute a ray cast on the \a node
        \param node The node where to execute the ray cast
        \return True if there is an intersection
    */
    bool                execute(VoxelNode<T_Area> const& node);

    /*!
        \brief Execute a raycast
        \param ray The ray to cast
        \param node Node where to execute the raycast
        \param predicate Function that allow to add some conditions in raytracing
        \param maxDistance Maximum distance where to execute the ray casting
        \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelNode<T_Area> const& node, Predicate const& predicate, double maxDistance = -1);
    /*!
        \brief Execute a raycast
        \param ray The ray to cast
        \param node Node where to execute the raycast
        \param maxDistance Maximum distance where to execute the ray casting
        \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelNode<T_Area> const& node, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param predicate Function that allow to add some conditions in raytracing
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelOctree<T_Area> const& octree, Predicate const& predicate, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelOctree<T_Area> const& octree, double maxDistance = -1);

    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param node Node where to execute the raycast
       \param cache The cache structure
       \param predicate Function that allow to add some conditions in raytracing
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelNode<T_Area> const& node, Cache& cache, Predicate const& predicate, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param node Node where to execute the raycast
       \param cache The cache structure
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelNode<T_Area> const& node, Cache& cache, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param cache The cache structure
       \param predicate Function that allow to add some conditions in raytracing
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelOctree<T_Area> const& octree, Cache& cache, Predicate const& predicate, double maxDistance = -1);
    /*!
       \brief Execute a raycast
       \param ray The ray to cast
       \param octree Octree where to execute the raycast
       \param cache The cache structure
       \param maxDistance Maximum distance where to execute the ray casting
       \return The ray cast result
    */
    static Result       get(Ray const& ray, VoxelOctree<T_Area> const& octree, Cache& cache, double maxDistance = -1);

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
    static Result       get(Ray const& ray, VoxelNode<T_Area> const* const* nodes, size_t nbNode, Cache& cache, Predicate const& predicate, double maxDistance = -1);

    Ray                 ray;                //!< The ray to cast
    double              maxDistance = -1;   //!< The maximum distance of the ray cast
    Predicate           predicate;          //!< Function that allow to add some conditions in raytracing
    Result              result;             //!< The ray cast result

private:
    /*!
        \brief Raycast on a voxel
        \param node Node where the voxel is
        \param pos Position of the voxel
        \return True if ray intersect the voxel
    */
    bool raycastVoxel(VoxelNode<T_Area> const& node, Vector3I const& pos);
    /*!
        \brief Raycast on a box
        \param node Node where the box is
        \param boxPosition Position of the box
        \param boxSize Size of the box
        \return True if ray intersect a voxel inside the box
    */
    bool raycastArea(VoxelNode<T_Area> const& node, Vector3I const& boxPosition, size_t boxSize);
    /*!
        \brief Raycast on a node
        \param node The node
        \return True if ray intersect a voxel inside the node
    */
    bool raycast(VoxelNode<T_Area> const& node);

    int                 _sortingIndex = 0; //!< Index inside hardcoded array, improve ray casting performance
    SideEnum            _sideToCheck = SideEnum::ALL; //!< Side to check with the ray cast
    Cache*              _cache = nullptr; //!< Cache structure, to improve performance
};

}

#include "Raycast.ipp"

#endif // _RAYCAST_HPP_