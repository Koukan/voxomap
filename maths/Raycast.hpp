#ifndef _VOXOMAP_RAYCAST_HPP_
#define _VOXOMAP_RAYCAST_HPP_

#include <functional>
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
    using Predicate = std::function<bool(VoxelData const&, Vector3I const&)>;

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

	bool				execute(VoxelNode<T_Area> const& node);

	static Result		get(Ray const& ray, VoxelNode<T_Area> const& octree, Predicate const& predicate, double maxDistance = -1);
	static Result		get(Ray const& ray, VoxelNode<T_Area> const& octree, double maxDistance = -1);
	static Result		get(Ray const& ray, VoxelOctree<T_Area> const& octree, Predicate const& predicate, double maxDistance = -1);
	static Result		get(Ray const& ray, VoxelOctree<T_Area> const& octree, double maxDistance = -1);

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
};

}

#include "Raycast.inl"

#endif // _RAYCAST_HPP_