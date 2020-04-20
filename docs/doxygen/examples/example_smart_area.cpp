/*!
\example SmartArea
\code
*/

#include <iostream>
#include "voxel_octree/VoxelOctree.hpp"
#include "voxel_octree/SmartArea.hpp"

// First, we begin by define a user data structure (voxel data)
struct UserData
{
    // Constructor
    UserData(int value = 0) : value(value)
    {
    }

    // Attribute of the voxel
    int64_t value = 0;
};

void main()
{
    // Create the voxel container
    // Use SmartArea as voxel leaf structure
    voxomap::VoxelOctree<voxomap::SmartArea<UserData>> voxel_octree;

    // Add a voxel at position 0, 0, 0 with value 42
    voxel_octree.addVoxel(0, 0, 0, 42);

    // Update value of voxel at position 0, 0, 0 with value 43
    voxel_octree.updateVoxel(0, 0, 0, 43);

    // Declare iterator
    voxomap::VoxelOctree<voxomap::SmartArea<UserData>>::iterator it;

    // Get iterator on voxel at position 0, 0, 0
    it = voxel_octree.findVoxel(0, 0, 0);

    if (it)
    {
        // Get the node where the voxel is
        voxomap::VoxelNode<voxomap::SmartArea<UserData>>* node;
        node = it.node;

        // Get the voxel area where the voxel is
        voxomap::SmartArea<UserData>* area = it.getArea();

        // Write the internal position of the voxel is inside the node and its value
        std::cout << "x: " << int(it.x) << " y: " << int(it.y) << " z: " << int(it.z) << " value: " << it.voxel->value << std::endl;
    }

    // Remove voxel at position 0, 0, 0
    voxel_octree.removeVoxel(0, 0, 0);
}

/*!
\endcode

Output:
\code
x: 0 y: 0 z: 0 value: 43
\endcode
*/