#ifndef _VOXOMAP_CACHEFRIENDLYALLOCATOR_HPP_
#define _VOXOMAP_CACHEFRIENDLYALLOCATOR_HPP_


#define CACHEALLOCATOR
#ifdef CACHEALLOCATOR
#include <array>
#include <memory>

namespace voxomap
{

unsigned int countTrailingZero(uint32_t nb)
{
#ifndef NO_INTRINSIC
#ifdef _WIN32
    return _tzcnt_u32(nb);
#else
    return __builtin_ctz(nb);
#endif
#else
    int count = 0;
    while ((nb & 1) == 0)
    {
        nb = nb >> 1;
        count++;
    }
    return count;
#endif
}

template <class T_Node>
class CacheFriendlyAllocator
{
public:
    static T_Node* allocate(T_Node const* parent, int x, int y, int z, uint32_t size)
    {
        auto node = _allocate(parent, x, y, z, size);

        new (node) T_Node(x, y, z, size);
        return node;
    }

    static T_Node* allocate(T_Node const* parent, T_Node const& other)
    {
        auto node = _allocate(parent, other.getX(), other.getY(), other.getZ(), other.getSize());

        new (node) T_Node(other);
        return node;
    }

    static T_Node* allocateFromChild(T_Node const& child, int x, int y, int z, uint32_t size)
    {
        static const uint32_t _odd = countTrailingZero(T_Node::Container::NB_VOXELS) & 1;

        if (child.getParent() && (countTrailingZero(size) & 1) == _odd)
            return allocate(child.getParent(), x, y, z, size);

        Data* data = nullptr;
        int shift = countTrailingZero(size);
        uint8_t id = 0;

        if ((shift & 1) == _odd)
        {
            id = (((x >> shift) & 1) | (((y >> shift) & 1) << 1) | (((z >> shift) & 1) << 2)) + 1;
            data = new Data();
        }
        else if (child.getSize() == (size >> 1))
        {
            --shift;
            uint8_t pid = (((child.getX() >> shift) & 1) | (((child.getY() >> shift) & 1) << 1) | (((child.getZ() >> shift) & 1) << 2)) + 1;
            data = reinterpret_cast<Data*>((char*)&child - (pid * sizeof(T_Node)));
        }
        else
            data = new Data();

        ++data->nb;
        new (&data->node[id]) T_Node(x, y, z, size);
        return &data->node[id];
    }

    static void deallocate(T_Node* ptr)
    {
        static const uint32_t _odd = countTrailingZero(T_Node::Container::NB_VOXELS) & 1;

        if (!T_Node::isNegPosRootNode(ptr->getX(), ptr->getSize()))
        {
            int shift = countTrailingZero(ptr->getSize());
            uint8_t id = (shift & 1) == _odd ? (((ptr->getX() >> shift) & 1) | (((ptr->getY() >> shift) & 1) << 1) | (((ptr->getZ() >> shift) & 1) << 2)) + 1 : 0;

            auto data = reinterpret_cast<Data*>((char*)ptr - (id * sizeof(T_Node)));
            data->node[id].~T_Node();
            --data->nb;
            if (data->nb == 0)
                delete data;
        }
        else
            delete ptr;
    }

private:
    static T_Node* _allocate(T_Node const* parent, int x, int y, int z, uint32_t size)
    {
        static const uint32_t _odd = countTrailingZero(T_Node::Container::NB_VOXELS) & 1;

        if (T_Node::isNegPosRootNode(x, size))
            return reinterpret_cast<T_Node*>(::operator new(sizeof(T_Node)));

        Data* data = nullptr;
        uint8_t id = 0;
        int shift = countTrailingZero(size);

        if ((shift & 1) == _odd)
            id = (((x >> shift) & 1) | (((y >> shift) & 1) << 1) | (((z >> shift) & 1) << 2)) + 1;

        if (parent != nullptr)
        {
            if (parent->getSize() == (size << 1))
            {
                if (id != 0)
                {
                    data = reinterpret_cast<Data*>((char*)parent);
                }
                else
                {
                    auto child = parent->getChildren()[id];
                    int sizeMask = ~(size - 1);
                    if (child && child->getSize() == (size >> 1) &&
                        x == (child->getX() & sizeMask) && 
                        y == (child->getY() & sizeMask) &&
                        z == (child->getZ() & sizeMask))
                    {
                        --shift;
                        uint8_t pid = (((child->getX() >> shift) & 1) | (((child->getY() >> shift) & 1) << 1) | (((child->getZ() >> shift) & 1) << 2)) + 1;
                        data = reinterpret_cast<Data*>((char*)child - (pid * sizeof(T_Node)));
                    }
                }
            }
            else
            {
                int pshift = countTrailingZero(parent->getSize() >> 1);
                uint8_t pid = ((x >> pshift) & 1) | (((y >> pshift) & 1) << 1) | (((z >> pshift) & 1) << 2);
                auto child = parent->getChildren()[pid];

                if (child)
                {

                    if (id != 0)
                    {
                        int sizeMask = ~((size << 1) - 1);

                        if (child->getSize() == size &&
                            (x & sizeMask) == (child->getX() & sizeMask) &&
                            (y & sizeMask) == (child->getY() & sizeMask) &&
                            (z & sizeMask) == (child->getZ() & sizeMask))
                        {
                            pid = (((child->getX() >> shift) & 1) | (((child->getY() >> shift) & 1) << 1) | (((child->getZ() >> shift) & 1) << 2)) + 1;
                            data = reinterpret_cast<Data*>((char*)child - (pid * sizeof(T_Node)));
                        }
                    }
                    else
                    {
                        int sizeMask = ~(size - 1);

                        if (child->getSize() == (size >> 1) &&
                            x == (child->getX() & sizeMask) &&
                            y == (child->getY() & sizeMask) &&
                            z == (child->getZ() & sizeMask))
                        {
                            --shift;
                            pid = (((child->getX() >> shift) & 1) | (((child->getY() >> shift) & 1) << 1) | (((child->getZ() >> shift) & 1) << 2)) + 1;
                            data = reinterpret_cast<Data*>((char*)child - (pid * sizeof(T_Node)));
                        }
                    }
                }
            }
        }
        
        if (!data)
            data = new Data();

        ++data->nb;
        return &data->node[id];
    }

    struct Data
    {
        Data() {}
        ~Data() {}

        /*static void* operator new(size_t size)
        {
            return ::concurrency::Alloc(size);
        }

        static void operator delete(void* p)
        {
            ::concurrency::Free(p);
        }*/

        union
        {
            T_Node node[9];
            int _;
        };
        size_t nb = 0;
    };
};

struct CacheFriendlyDeleter
{
    template <typename T_Node>
    void operator()(T_Node* ptr)
    {
        CacheFriendlyAllocator<T_Node>::deallocate(ptr);
    }
};

}
#endif

#endif // _VOXOMAP_CACHEFRIENDLYALLOCATOR_HPP_
