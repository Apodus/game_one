#pragma once

#include "Types.hpp"
#include "Vector.hpp"

namespace bs
{
	struct Visualization
	{
		struct Addition
		{
			Real radius;
			Unit::Id id;
			uint8_t team;
		};

		struct Movement
		{
			Vec pos;
			U32 hitpoints;
			Unit::Id id;
		};

		struct Removal
		{
			Unit::Id id;
		};

		struct Hitpoints
		{
			Unit::Id id;
			uint8_t newValue;
		};

		struct Reader
		{
			Reader(const Visualization& ud) : 
				buffer(&ud.updateStream[0]), readPos(0)
			{
			}

			template<typename T>
			const T& Read()
			{
				auto* p = reinterpret_cast<const T*>(&buffer[readPos]);
				readPos += sizeof(T);
				return *p;
			}

			const U8* buffer;
			size_t readPos;
		};

		struct Writer
		{
			Writer(Visualization& ud) :
				buffer(&ud.updateStream[0]), writePos(0)
			{
			}

			template<typename T>
			void Write(const T& data)
			{
				Write(static_cast<const void*>(&data), sizeof(T));
			}

			void Write(const void* data, size_t len)
			{
				memcpy(&buffer[writePos], data, len);
				writePos += len;
			}

			template<typename T>
			T& Write()
			{
				auto* p = reinterpret_cast<T*>(&buffer[writePos]);
				writePos += sizeof(T);
				return *p;
			}

			U8* buffer;
			size_t writePos;
		};

		size_t Align(size_t pos, size_t alignment)
		{
			size_t offset = (reinterpret_cast<size_t>(&updateStream[0]) + pos) % alignment;
			return pos + offset;
		}

		Writer GetWriter() { return Writer(*this); }

		Reader GetReader() const { return Reader(*this); }

	private:
		friend class VisualizationSystem;

		Vector<U8> updateStream;
	};
}