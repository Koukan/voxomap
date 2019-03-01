#ifndef _VOXOMAP_RAYCAST_HPP_
#define _VOXOMAP_RAYCAST_HPP_

#include <functional>
#include <unordered_map>
#include <bitset>
#include "Ray.hpp"
#include "../voxel_octree/FaceArea.hpp"
#include "../voxel_octree/VoxelOctree.hpp"

namespace voxomap
{

template <class T_Area>
class Raycast
{
public:
    using VoxelData = typename T_Area::VoxelData;
    using Predicate = std::function<bool(VoxelNode<T_Area> const&, VoxelData const&, Vector3I const&)>;

    struct Result
    {
        bool				operator<(Result const& other) const;
        bool				operator>(Result const& other) const;

        Vector3D					position;
        Vector3D					voxelPosition;
        double						distance = -1;
        VoxelNode<T_Area> const*	node = nullptr;
        VoxelData const*			voxel = nullptr;
        FaceEnum					face = FaceEnum::TOP;
    };

    struct Cache
    {
        struct NodeCache
        {
            std::bitset<8>      hasVoxelIn4;
            std::bitset<64>     hasVoxelIn2;
        };
        bool hasVoxel(VoxelNode<T_Area> const& node, Vector3I const& boxPosition, size_t boxSize) const;
        bool fillHasVoxel(VoxelNode<T_Area> const& node, Vector3I const& boxPosition, size_t boxSize);
        void fillCache(VoxelNode<T_Area> const& node);
        void fillCache(VoxelOctree<T_Area> const& octree);

    private:
        void fillCache(VoxelNode<T_Area> const& node, NodeCache& cache);

        std::unordered_map<VoxelNode<T_Area> const*, NodeCache>    _nodeCache;
    };

	bool				execute(VoxelNode<T_Area> const& node);

	static Result		get(Ray const& ray, VoxelNode<T_Area> const& node, Predicate const& predicate, double maxDistance = -1);
	static Result		get(Ray const& ray, VoxelNode<T_Area> const& node, double maxDistance = -1);
	static Result		get(Ray const& ray, VoxelOctree<T_Area> const& octree, Predicate const& predicate, double maxDistance = -1);
	static Result		get(Ray const& ray, VoxelOctree<T_Area> const& octree, double maxDistance = -1);

    static Result		get(Ray const& ray, VoxelNode<T_Area> const& node, Cache& cache, Predicate const& predicate, double maxDistance = -1);
    static Result		get(Ray const& ray, VoxelNode<T_Area> const& node, Cache& cache, double maxDistance = -1);
    static Result		get(Ray const& ray, VoxelOctree<T_Area> const& octree, Cache& cache, Predicate const& predicate, double maxDistance = -1);
    static Result		get(Ray const& ray, VoxelOctree<T_Area> const& octree, Cache& cache, double maxDistance = -1);

    static Result		get(Ray const& ray, VoxelNode<T_Area> const* const* nodes, size_t nbNode, Cache& cache, Predicate const& predicate, double maxDistance = -1);

    Ray                 ray;
	double	            maxDistance = -1;
    Predicate           predicate;
    Result              result;

private:
	bool raycastVoxel(VoxelNode<T_Area> const& node, Vector3I const& pos);
	bool raycastArea(VoxelNode<T_Area> const& node, Vector3I const& boxPosition, size_t boxSize);
	bool raycast(VoxelNode<T_Area> const& node);

    int                 _sortingIndex = 0;
    FaceEnum            _faceToCheck = FaceEnum::ALL;
    Cache*              _cache = nullptr;
};

}

#include "Raycast.inl"

#endif // _RAYCAST_HPP_