#ifndef _VOXOMAP_VOXELOCTREE_HPP_
#define _VOXOMAP_VOXELOCTREE_HPP_

#include <unordered_set>
#include "../octree/Octree.hpp"
#include "VoxelArea.hpp"
#include "VoxelNode.hpp"

namespace voxomap
{

template <class T_Area> class VoxelNode;

/*!
    \brief Octree optimized for voxel
*/
template <class T_Area>
class VoxelOctree : public Octree<VoxelNode<T_Area>>
{
public:
    using VoxelData = typename T_Area::VoxelData;

    /*!
        \brief Constructs VoxelOctree
        \param voxelSize Size of voxels
    */
    VoxelOctree();
    VoxelOctree(VoxelOctree<T_Area> const& other);
    virtual ~VoxelOctree() = default;

    VoxelNode<T_Area>*                  push(VoxelNode<T_Area>& node) override;
    std::unique_ptr<VoxelNode<T_Area>>  pop(VoxelNode<T_Area>& node) override;
    void                                clear() override;
    /*!
    \brief Returns a voxel, can be NULL
    \param position Position of the voxel
    \param ret The node which contain the voxel
    */
    VoxelData*              getVoxelAt(int x, int y, int z, VoxelNode<T_Area>** ret = nullptr) const;
    
    /*!
        \brief Add the voxel if not exist
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param args Arguments forward to VoxelData constructor
    */
    template <typename... Args>
    VoxelData*              addVoxel(int x, int y, int z, Args&&... args);
    /*!
        \brief Update the voxel if already exist
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param args Arguments forward to VoxelData constructor
    */
    template <typename... Args>
    VoxelData*              updateVoxel(int x, int y, int z, Args&&... args);
    /*!
        \brief Add or update the voxel
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param args Arguments forward to VoxelData constructor
    */
    template <typename... Args>
    VoxelData*              putVoxel(int x, int y, int z, Args&&... args);

    /*!
        \brief Removes a voxel
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
    */
    bool                    removeVoxel(int x, int y, int z);
    /*!
    \brief Removes a voxel
    \param x X coordinate
    \param y Y coordinate
    \param z Z coordinate
    \return Voxel data
    */
    bool                    removeVoxel(int x, int y, int z, VoxelData& data);

    /*!
        \brief Returns the size of areas
    */
    unsigned int            getAreaSize() const;

    /*!
        \brief Enables/Disables the call to the method updateOctree
    */
    void                    activeUpdate(bool enable = true);
    /*!
        \brief Is called when \a node is modified
    */
    void                    updateOctree(VoxelNode<T_Area>& node);

    /*!
        \brief Calcul the bounding box
    */
    //void                    calculBoundingBox(Core::RectHitbox &hitbox) const;
    void                    removeOfCache(VoxelNode<T_Area> const& node);

    //void                    getVoxels(std::vector<VoxelInfo> &voxels, Core::Vector3I const &offset = Core::Vector3I(0, 0, 0)) const;
    void                    addUpdateCallback(std::string const& name, std::function<void(VoxelNode<T_Area>&)> const& func);

    void                    exploreVoxel(std::function<void(VoxelNode<T_Area> const&, VoxelData const&, uint8_t, uint8_t, uint8_t)> const& predicate) const;
    void                    exploreVoxelArea(std::function<void(VoxelNode<T_Area> const&)> const& predicate) const;

    /*!
        \brief getter and setter for the number of voxels
    */
    unsigned int            getNbVoxels() const;
    void                    setNbVoxels(unsigned int nbVoxels);

protected:
    /*!
        \brief Adds the area nodes, an area represent a chunck of voxels
    */
    virtual VoxelNode<T_Area>*  pushAreaNode(int x, int y, int z);

protected:
    struct NodeCache
    {
        VoxelNode<T_Area>*                      node = nullptr;
        std::unordered_set<VoxelNode<T_Area>*>  cache;
    };

    template <typename T>
    using CallbackList = std::vector<std::pair<std::function<T>, std::string>>;

    template <typename T, typename... Args>
    void callback(CallbackList<T> const& list, Args&&... args);

    bool                                        _activeUpdate = true;            //!< The call to the method updateOctree is it enable?
    CallbackList<void(VoxelNode<T_Area>&)>      _updateNodeCallbacks;
    NodeCache                                   _nodeCache;
    mutable VoxelArea<T_Area>                   _cache;                            //!< Cache for improve performance
    unsigned int                                _nbVoxels = 0;

protected:
    void                    callUpdate(VoxelNode<T_Area>& node, std::array<VoxelNode<T_Area>*, 6> const& nodes);
    friend VoxelNode<T_Area>;

private:
    void                    callUpdate(VoxelNode<T_Area>& node);
};

}

#include "VoxelOctree.ipp"

#endif // _VOXOMAP_VOXELOCTREE_HPP_
