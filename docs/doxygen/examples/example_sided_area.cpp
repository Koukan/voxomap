/*!
\example SidedArea
\code
*/

#include <iostream>
#include "voxel_octree/VoxelOctree.hpp"
#include "voxel_octree/SmartArea.hpp"
#include "voxel_octree/SidedArea.hpp"

// First, we begin by define a user data structure (voxel data)
struct UserData
{
    // Constructor
    UserData(int value = 0) : value(value)
    {
    }

    // Method used by SidedArea to know if we have to merge the side between two neighbor voxel
    bool mergeSide(UserData const& other) const
    {
        return true;
    }

    // Attribute of the voxel
    int64_t value = 0;
};

// Typedef SmartArea to be use with SidedArea
template <typename T_Voxel>
using TypedefSmartArea = voxomap::SmartArea<T_Voxel>;

void main()
{
    // Create the voxel container
    // Use SidedArea with SmartArea as voxel leaf structure
    voxomap::VoxelOctree<voxomap::SidedArea<TypedefSmartArea, UserData>> voxel_octree;

    // Add voxels from position 0, 0, 0 and 0, 1, 0 and 0, 0, 1
    voxel_octree.addVoxel(0, 0, 0, 42);
    voxel_octree.addVoxel(0, 1, 0, 1);
    voxel_octree.addVoxel(0, 0, 1, 2);

    // Declare iterator
    voxomap::VoxelOctree<voxomap::SidedArea<TypedefSmartArea, UserData>>::iterator it;

    // Get iterator on voxel at position 0, 0, 0
    it = voxel_octree.findVoxel(0, 0, 0);

    if (it)
    {
        // Get the node where the voxel is
        voxomap::VoxelNode<voxomap::SidedArea<TypedefSmartArea, UserData>>* node;
        node = it.node;

        // Get the voxel area where the voxel is
        voxomap::SidedArea<TypedefSmartArea, UserData>* area = it.getArea();

        // Get the voxel pointer
        voxomap::SidedVoxel<UserData>* voxel = it.voxel;

        // Write number of side inside the area
        std::cout << "Number of side: " << area->getNbSide() << std::endl;

        // Write the internal position of the voxel is inside the node and its value
        std::cout << "x: " << int(it.x) << " y: " << int(it.y) << " z: " << int(it.z) << " value: " << voxel->value << " side: " << int(voxel->getSide()) << std::endl;

        // Use operator & of SidedVoxel structure
        if (*voxel & voxomap::SideEnum::YPOS)
        {
            auto tmp_it = node->findVoxel(0, 1, 0);

            // Write the internal position of the voxel is inside the node and its value
            std::cout << "x: " << int(tmp_it.x) << " y: " << int(tmp_it.y) << " z: " << int(tmp_it.z) << " value: " << tmp_it.voxel->value << " side: " << int(tmp_it.voxel->getSide()) << std::endl;
        }
    }
}

/*!
\endcode

Output:
\code
Number of side: 14
x: 0 y: 0 z: 0 value: 42 side: 20
x: 0 y: 1 z: 0 value: 1 side: 8
\endcode
*/