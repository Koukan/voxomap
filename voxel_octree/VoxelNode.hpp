#ifndef _VOXOMAP_VOXELNODE_HPP_
#define _VOXOMAP_VOXELNODE_HPP_

#include <cstdint>
#include <memory>
#include <functional>
#include "../octree/Node.hpp"
#include "../maths/BoundingBox.hpp"

namespace voxomap
{

template <class T_Area> class VoxelOctree;

template <class T_Area>
class VoxelNode : public Node<VoxelNode<T_Area>>
{
public:
    using VoxelData = typename T_Area::VoxelData;
    using iterator = typename T_Area::iterator;
    using P_Node = Node<VoxelNode<T_Area>>;
    const static uint32_t VOXEL_MASK = T_Area::NB_VOXELS - 1;
    const static uint32_t AREA_MASK = ~VOXEL_MASK;

    VoxelNode(int x, int y, int z, int size);
    VoxelNode(VoxelNode const& other);
    virtual ~VoxelNode() = default;

    iterator begin();
    iterator end();

    unsigned int            getNbVoxel() const;
    bool                    hasVoxel() const;
    T_Area*                 getVoxelArea();
    T_Area const*           getVoxelArea() const;
    std::shared_ptr<T_Area> getSharedVoxelArea();
    void                    setVoxelArea(std::shared_ptr<T_Area> area);
    iterator                findVoxel(int x, int y, int z);
    bool                    findVoxel(iterator& it);
    template <typename... Args>
    bool                    addVoxel(iterator& it, Args&&... args);
    template <typename... Args>
    bool                    updateVoxel(iterator& it, Args&&... args);
    template <typename... Args>
    void                    putVoxel(iterator& it, Args&&... args);
    template <typename... Args>
    iterator                removeVoxel(iterator it, Args&&... args);

    void                    exploreVoxel(std::function<void(VoxelNode const&, VoxelData const&, uint8_t, uint8_t, uint8_t)> const& predicate) const;
    void                    exploreVoxelArea(std::function<void(VoxelNode const&)> const& predicate) const;
    void                    exploreVoxelArea(std::function<void(VoxelNode&)> const& predicate);

    void                    exploreBoundingBox(BoundingBox<int> const& bounding_box,
                                               std::function<void(VoxelNode&)> const& in_predicate,
                                               std::function<void(VoxelNode&)> const& out_predicate);

    void                    copyOnWrite();
    void                    merge(VoxelNode& node);
    bool                    empty() const;
    void                    serialize(std::string& str) const;
    static VoxelNode*       unserialize(char const* str, size_t strsize);

private:
    void                    serializeNode(VoxelNode const& node, std::string& str) const;
    
    std::shared_ptr<T_Area> _area;
    friend T_Area;

public:
    inline static int       findPosition(int src);
};

}

#include "VoxelNode.ipp"

#endif // _VOXOMAP_VOXELNODE_HPP_
