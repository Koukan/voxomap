#ifndef _VOXOMAP_VOXELNODE_HPP_
#define _VOXOMAP_VOXELNODE_HPP_

#include <cstdint>
#include <memory>
#include <functional>
#include "../octree/Node.hpp"

namespace voxomap
{

template <class T_Area> class VoxelOctree;

template <class T_Area>
class VoxelNode : public Node<VoxelNode<T_Area>>
{
public:
    using VoxelData = typename T_Area::VoxelData;
    using P_Node = Node<VoxelNode<T_Area>>;
	const static uint32_t VOXEL_MASK = T_Area::NB_VOXELS - 1;
	const static uint32_t AREA_MASK = ~VOXEL_MASK;

	VoxelNode(int x, int y, int z, int size);
	VoxelNode(VoxelNode const& other);
	virtual ~VoxelNode() = default;

	unsigned int		getNbVoxel() const;
	bool                hasVoxel() const;
	T_Area const*       getVoxelArea() const;
	std::shared_ptr<T_Area> getSharedVoxelArea();
    VoxelData const*    getVoxel(int x, int y, int z) const;
    VoxelData*          getVoxel(int x, int y, int z);
	VoxelData*			getVoxelAt(int x, int y, int z, VoxelNode** ret = nullptr) const;
    bool				getVoxelAt(int x, int y, int z, VoxelData*& voxel, VoxelNode** ret = nullptr) const;
    template <typename... Args>
    VoxelData*          addVoxel(int x, int y, int z, Args&&... args);
    template <typename... Args>
    VoxelData*          updateVoxel(int x, int y, int z, Args&&... args);
    template <typename... Args>
    VoxelData*          putVoxel(int x, int y, int z, Args&&... args);
    bool                removeVoxel(int x, int y, int z);
    bool                removeVoxel(int x, int y, int z, VoxelData& data);

	void                exploreVoxel(std::function<void(VoxelNode const&, VoxelData const&, uint8_t, uint8_t, uint8_t)> const& predicate) const;
    void                exploreVoxelArea(std::function<void(VoxelNode const&)> const& predicate) const;

    void            	copyOnWrite();
	void				merge(VoxelNode& node);
	bool				empty() const;
	void			    serialize(std::string& str) const;
	static VoxelNode*	unserialize(char const* str, size_t strsize);

private:
	void				serializeNode(VoxelNode const& node, std::string& str) const;
    
    std::shared_ptr<T_Area>     area;
    friend T_Area;

public:
    inline static int	findPosition(int src)
    {
		return src & VOXEL_MASK;
    }
};

}

#include "VoxelNode.inl"

#endif // _VOXOMAP_VOXELNODE_HPP_
