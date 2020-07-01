#ifndef _VOXOMAP_ITERATOR_HPP_
#define _VOXOMAP_ITERATOR_HPP_

#include <cstdint>

namespace voxomap
{

template <typename T_SuperContainer> class VoxelNode;

template <typename T>
struct container_iterator
{
	using VoxelData = typename T::VoxelData;
	using VoxelContainer = typename T::VoxelContainer;

	VoxelNode<T>* node = nullptr;				//!< Pointer on node
	VoxelContainer* voxel_container = nullptr;	//!< Pointer on voxel container
	VoxelData* voxel = nullptr;					//!< Pointer on voxel
	uint8_t x = 0;								//!< x coordinate of \a voxel inside \a node
	uint8_t y = 0;								//!< y coordinate of \a voxel inside \a node
	uint8_t z = 0;								//!< z coordinate of \a voxel inside \a node

	/*!
		\brief Pre-increment the iterator
		\return Reference on \a this
	*/
	container_iterator& operator++();
	/*!
		\brief Post increment the iterator
		\return Not incremented iterator
	*/
	container_iterator operator++(int);
	/*!
		\brief Dereference iterator
	*/
	container_iterator* operator*();
	/*!
		\brief Returns true if the iterator is valid
	*/
	operator bool() const;

	void findNextParentNode(VoxelNode<T>& i_node);
	bool findNextChildNode(VoxelNode<T>& i_node);
	bool findNextVoxel(VoxelContainer& container);

    /*!
        \brief Initialize \a this with the first voxel of \a node
    */
    void begin(VoxelNode<T>& node);
    /*!
        \brief Initialize \a this with the next element after the last voxel of \a node
    */
    void end(VoxelNode<T>& node);

    void getVoxelPosition(int& x, int& y, int& z) const;
};

template <typename T>
struct supercontainer_iterator : container_iterator<T>
{
	using Container = typename T::Container;
	using VoxelContainer = typename container_iterator<T>::VoxelContainer;
	using Position = std::tuple<uint8_t, uint8_t, uint8_t>;

	supercontainer_iterator& operator++();
	supercontainer_iterator operator++(int);
    supercontainer_iterator* operator*();

	bool findNextChildNode(VoxelNode<T>& i_node);

	void findNextParentNode(VoxelNode<T>& i_node);

	template <class T_Container>
    typename std::enable_if<(T_Container::NB_SUPERCONTAINER != 0 && T_Container::SUPERCONTAINER_ID != 0), bool>::type findNextContainer(T_Container& container);

    template <class T_Container>
    typename std::enable_if<(T_Container::NB_SUPERCONTAINER != 0 && T_Container::SUPERCONTAINER_ID == 0), bool>::type findNextContainer(T_Container& container);

	template <class T_Container>
    typename std::enable_if<(T_Container::NB_SUPERCONTAINER == 0), bool>::type findNextContainer(T_Container& container);

	/*!
		\brief Initialize \a this with the first voxel of \a node
	*/
	void begin(VoxelNode<T>& node);
	/*!
		\brief Initialize \a this with the next element after the last voxel of \a node
	*/
	void end(VoxelNode<T>& node);

    void getVoxelPosition(int& x, int& y, int& z) const;

	std::array<Position, T::NB_SUPERCONTAINER>	container_position;
};

}

#include "iterator.ipp"

#endif // _VOXOMAP_ITERATOR_HPP_
