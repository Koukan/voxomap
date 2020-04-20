/*!
\example "Find Voxel"
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

    // Add voxels from position 0, 0, 0 to 42, 42, 42
    int user_value = 1;
    for (int x = 0; x < 42; ++x)
    {
        for (int y = 0; y < 42; ++y)
        {
            for (int z = 0; z < 42; ++z)
            {
                voxel_octree.addVoxel(x, y, z, user_value);
                ++user_value;
            }
        }
    }

    // Declare iterator
    voxomap::VoxelOctree<voxomap::SmartArea<UserData>>::iterator it;

    // Get iterator on the voxel at position 10, 11, 12
    it = voxel_octree.findVoxel(10, 11, 12);

    if (it)
    {
        // Get the node where the voxel is
        voxomap::VoxelNode<voxomap::SmartArea<UserData>>* node;
        node = it.node;

        // Get the voxel area where the voxel is
        voxomap::SmartArea<UserData>* area = it.getArea();

        // Write the internal position of the voxel is inside the node and its value
        std::cout << "x: " << int(it.x) << " y: " << int(it.y) << " z: " << int(it.z) << " value: " << it.voxel->value << std::endl;

        // Get iterator on the voxel at position -1, 0, 1 relative to the node position
        auto tmp_it = node->findRelativeVoxel(-1, 0, 1);

        // Get iterator on voxels around the voxel at position 10, 11, 12
        for (int ix = -1; ix <= 1; ++ix)
        {
            for (int iy = -1; iy <= 1; ++iy)
            {
                for (int iz = -1; iz <= 1; ++iz)
                {
                    // Avoid to search the voxel at position 10, 11, 12
                    if (ix == 0 && iy == 0 && iz == 0)
                        continue;

                    // Find voxel at position 10 + ix, 11 + iy, 12 + iz
                    tmp_it = node->findRelativeVoxel(ix + it.x, iy + it.y, iz + it.z);

                    std::cout << "x: " << int(tmp_it.x) << " y: " << int(tmp_it.y) << " z: " << int(tmp_it.z) << " value: " << tmp_it.voxel->value << std::endl;

                }
            }
        }
    }
}
/*!
\endcode

Output:
\code
x: 2 y: 3 z: 4 value: 18115
x: 1 y: 2 z: 3 value: 16308
x: 1 y: 2 z: 4 value: 16309
x: 1 y: 2 z: 5 value: 16310
x: 1 y: 3 z: 3 value: 16350
x: 1 y: 3 z: 4 value: 16351
x: 1 y: 3 z: 5 value: 16352
x: 1 y: 4 z: 3 value: 16392
x: 1 y: 4 z: 4 value: 16393
x: 1 y: 4 z: 5 value: 16394
x: 2 y: 2 z: 3 value: 18072
x: 2 y: 2 z: 4 value: 18073
x: 2 y: 2 z: 5 value: 18074
x: 2 y: 3 z: 3 value: 18114
x: 2 y: 3 z: 5 value: 18116
x: 2 y: 4 z: 3 value: 18156
x: 2 y: 4 z: 4 value: 18157
x: 2 y: 4 z: 5 value: 18158
x: 3 y: 2 z: 3 value: 19836
x: 3 y: 2 z: 4 value: 19837
x: 3 y: 2 z: 5 value: 19838
x: 3 y: 3 z: 3 value: 19878
x: 3 y: 3 z: 4 value: 19879
x: 3 y: 3 z: 5 value: 19880
x: 3 y: 4 z: 3 value: 19920
x: 3 y: 4 z: 4 value: 19921
x: 3 y: 4 z: 5 value: 19922
\endcode
*/