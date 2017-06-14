#pragma once

////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
//
// Copyright (c) 2017 Nicholas Frechette & Animation Compression Library contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "acl/core/compressed_clip.h"
#include "acl/core/memory.h"

#include <stdint.h>

namespace acl
{
	namespace uniformly_sampled
	{
		namespace impl
		{
			struct FullPrecisionConstants
			{
				static constexpr uint32_t NUM_TRACKS_PER_BONE = 2;
				static constexpr uint32_t BITSET_WIDTH = 32;
			};

			struct FullPrecisionHeader
			{
				uint16_t				num_bones;
				RotationFormat8			rotation_format;
				VectorFormat8			translation_format;
				uint32_t				num_samples;
				uint32_t				sample_rate;								// TODO: Store duration as float instead
				uint32_t				num_animated_rotation_tracks;				// TODO: Calculate from bitsets?
				uint32_t				num_animated_translation_tracks;			// TODO: Calculate from bitsets?

				PtrOffset16<uint32_t>	default_tracks_bitset_offset;
				PtrOffset16<uint32_t>	constant_tracks_bitset_offset;
				PtrOffset16<uint8_t>	constant_track_data_offset;
				PtrOffset16<uint8_t>	track_data_offset;

				//////////////////////////////////////////////////////////////////////////

				uint32_t*		get_default_tracks_bitset()			{ return default_tracks_bitset_offset.add_to(this); }
				const uint32_t*	get_default_tracks_bitset() const	{ return default_tracks_bitset_offset.add_to(this); }

				uint32_t*		get_constant_tracks_bitset()		{ return constant_tracks_bitset_offset.add_to(this); }
				const uint32_t*	get_constant_tracks_bitset() const	{ return constant_tracks_bitset_offset.add_to(this); }

				uint8_t*		get_constant_track_data()			{ return constant_track_data_offset.add_to(this); }
				const uint8_t*	get_constant_track_data() const		{ return constant_track_data_offset.add_to(this); }

				uint8_t*		get_track_data()					{ return track_data_offset.add_to(this); }
				const uint8_t*	get_track_data() const				{ return track_data_offset.add_to(this); }
			};

			constexpr FullPrecisionHeader& get_full_precision_header(CompressedClip& clip)
			{
				return *add_offset_to_ptr<FullPrecisionHeader>(&clip, sizeof(CompressedClip));
			}

			constexpr const FullPrecisionHeader& get_full_precision_header(const CompressedClip& clip)
			{
				return *add_offset_to_ptr<const FullPrecisionHeader>(&clip, sizeof(CompressedClip));
			}

			// TODO: constexpr
			inline uint32_t get_rotation_size(RotationFormat8 rotation_format)
			{
				switch (rotation_format)
				{
					case RotationFormat8::Quat_128:	return sizeof(float) * 4;
					case RotationFormat8::Quat_96:	return sizeof(float) * 3;
					case RotationFormat8::Quat_48:	return sizeof(uint16_t) * 3;
					case RotationFormat8::Quat_32:	return sizeof(uint32_t);
					default:
						ACL_ENSURE(false, "Invalid or unsupported rotation format: %s", get_rotation_format_name(rotation_format));
						return 0;
				}
			}

			// TODO: constexpr
			inline uint32_t get_translation_size(VectorFormat8 translation_format)
			{
				switch (translation_format)
				{
				case VectorFormat8::Vector3_96:		return sizeof(float) * 3;
				case VectorFormat8::Vector3_48:		return sizeof(uint16_t) * 3;
				default:
					ACL_ENSURE(false, "Invalid or unsupported translation format: %s", get_vector_format_name(translation_format));
					return 0;
				}
			}

			constexpr uint32_t get_bitset_size(uint32_t num_bits)
			{
				return (num_bits + FullPrecisionConstants::BITSET_WIDTH - 1) / FullPrecisionConstants::BITSET_WIDTH;
			}
		}
	}
}