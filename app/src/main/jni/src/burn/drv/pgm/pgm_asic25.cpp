/*
	IGS Asic25 + (Asic12, Asic22, or Asic28) emulation

	Used by:
		Oriental Legends Special (Asic25 + Asic28)
		The Killing Blade (Asic25 + Asic22)
		Dragon World 2 (Asic25 + Asic12)
		Dragon World 3 (Asic25 + Asic22)
*/

#include "pgm.h"
#include "bitswap.h"

static const UINT8 source_data[0x22][0xec] =
{
	{ 0, }, // Region 0, not used
	{ // region 1, $14c21a
		0x67, 0x51, 0xf3, 0x19, 0xa0, 0x09, 0xb1, 0x21, 0xb0, 0xee, 0xe3, 0xf6, 0xbe, 0x81, 0x35, 0xe3,
		0xfb, 0xe6, 0xef, 0xdf, 0x61, 0x01, 0xfa, 0x22, 0x5d, 0x43, 0x01, 0xa5, 0x3b, 0x17, 0xd4, 0x74,
		0xf0, 0xf4, 0xf3, 0x43, 0xb5, 0x19, 0x04, 0xd5, 0x84, 0xce, 0x87, 0xfe, 0x35, 0x3e, 0xc4, 0x3c,
		0xc7, 0x85, 0x2a, 0x33, 0x00, 0x86, 0xd0, 0x4d, 0x65, 0x4b, 0xf9, 0xe9, 0xc0, 0xba, 0xaa, 0x77,
		0x9e, 0x66, 0xf6, 0x0f, 0x4f, 0x3a, 0xb6, 0xf1, 0x64, 0x9a, 0xe9, 0x25, 0x1a, 0x5f, 0x22, 0xa3,
		0xa2, 0xbf, 0x4b, 0x77, 0x3f, 0x34, 0xc9, 0x6e, 0xdb, 0x12, 0x5c, 0x33, 0xa5, 0x8b, 0x6c, 0xb1,
		0x74, 0xc8, 0x40, 0x4e, 0x2f, 0xe7, 0x46, 0xae, 0x99, 0xfc, 0xb0, 0x55, 0x54, 0xdf, 0xa7, 0xa1,
		0x0f, 0x5e, 0x49, 0xcf, 0x56, 0x3c, 0x90, 0x2b, 0xac, 0x65, 0x6e, 0xdb, 0x58, 0x3e, 0xc9, 0x00,
		0xae, 0x53, 0x4d, 0x92, 0xfa, 0x40, 0xb2, 0x6b, 0x65, 0x4b, 0x90, 0x8a, 0x0c, 0xe2, 0xa5, 0x9a,
		0xd0, 0x20, 0x29, 0x55, 0xa4, 0x44, 0xac, 0x51, 0x87, 0x54, 0x53, 0x34, 0x24, 0x4b, 0x81, 0x67,
		0x34, 0x4c, 0x5f, 0x31, 0x4e, 0xf2, 0xf1, 0x19, 0x18, 0x1c, 0x34, 0x38, 0xe1, 0x81, 0x17, 0xcf,
		0x24, 0xb9, 0x9a, 0xcb, 0x34, 0x51, 0x50, 0x59, 0x44, 0xb1, 0x0b, 0x50, 0x95, 0x6c, 0x48, 0x7e,
		0x14, 0xa4, 0xc6, 0xd9, 0xd3, 0xa5, 0xd6, 0xd0, 0xc5, 0x97, 0xf0, 0x45, 0xd0, 0x98, 0x51, 0x91,
		0x9f, 0xa3, 0x43, 0x51, 0x05, 0x90, 0xee, 0xca, 0x7e, 0x5f, 0x72, 0x53, 0xb1, 0xd3, 0xaf, 0x36,
		0x08, 0x75, 0xb0, 0x9b, 0xe0, 0x0d, 0x43, 0x88, 0xaa, 0x27, 0x44, 0x11
	},
	{ // region 2, $14c126
		0xf9, 0x19, 0xf3, 0x09, 0xa0, 0x09, 0xb0, 0x21, 0xb0, 0x22, 0xfd, 0x8e, 0xd3, 0xc8, 0x31, 0x67,
		0xc0, 0x10, 0x3c, 0xc2, 0x03, 0xf2, 0x6a, 0x0a, 0x54, 0x49, 0xca, 0xb5, 0x4b, 0xe0, 0x94, 0xe8,
		0x8d, 0xc8, 0x90, 0xee, 0x6b, 0x6f, 0xfa, 0x09, 0x76, 0x84, 0x6f, 0x55, 0xd1, 0x94, 0xca, 0x9c,
		0xe1, 0x22, 0xc6, 0x02, 0xb5, 0x8c, 0xf9, 0x3a, 0x52, 0x10, 0xf0, 0x22, 0xe4, 0x11, 0x15, 0x73,
		0x5e, 0x9e, 0xde, 0xc4, 0x5a, 0xbd, 0xa3, 0x89, 0xe7, 0x9b, 0x95, 0x5d, 0x75, 0xf6, 0xc3, 0x9f,
		0xe4, 0xcf, 0x65, 0x73, 0x90, 0xd0, 0x75, 0x56, 0xfa, 0xcc, 0xe4, 0x3e, 0x9c, 0x41, 0x81, 0x62,
		0xb1, 0xd3, 0x28, 0xbd, 0x6c, 0xed, 0x60, 0x28, 0x27, 0xee, 0xf2, 0xa1, 0xb4, 0x2c, 0x6c, 0xbb,
		0x42, 0xd7, 0x1d, 0x62, 0xc0, 0x33, 0x7d, 0xf9, 0xe4, 0x5c, 0xe2, 0x41, 0xa4, 0x1c, 0x98, 0xa1,
		0x87, 0x95, 0xad, 0x61, 0x56, 0x96, 0x40, 0x08, 0x6b, 0xe2, 0x4b, 0x95, 0x7b, 0x1b, 0xd8, 0x64,
		0xb3, 0xee, 0x9d, 0x79, 0x69, 0xea, 0x5d, 0xcf, 0x01, 0x91, 0xea, 0x3f, 0x70, 0x29, 0xdc, 0xe0,
		0x08, 0x20, 0xbf, 0x46, 0x90, 0xa8, 0xfc, 0x29, 0x14, 0xd1, 0x0d, 0x20, 0x79, 0xd2, 0x2c, 0xe9,
		0x52, 0xa6, 0x8c, 0xbd, 0xa3, 0x3e, 0x88, 0x2d, 0xb8, 0x4e, 0xf2, 0x74, 0x50, 0xcc, 0x12, 0xde,
		0xd3, 0x5a, 0xa4, 0x7b, 0xa2, 0x8d, 0x91, 0x68, 0x12, 0x0c, 0x9c, 0xb9, 0x6d, 0x26, 0x66, 0x60,
		0xc3, 0x6d, 0xd0, 0x11, 0x33, 0x05, 0x1d, 0xa8, 0xb6, 0x51, 0xe6, 0xe0, 0x58, 0x61, 0x74, 0x37,
		0xcc, 0x3a, 0x4d, 0x6a, 0x0a, 0x09, 0x71, 0xe3, 0x7e, 0xa5, 0x3b, 0xe9
	},
	{ // region 3, $14E5BE
		0x73, 0x59, 0xf3, 0x09, 0xa0, 0x09, 0xb1, 0x21, 0xb0, 0x55, 0x18, 0x0d, 0xe8, 0x29, 0x2d, 0x04,
		0x85, 0x39, 0x88, 0xbe, 0x8b, 0xcb, 0xd9, 0x0b, 0x32, 0x36, 0x94, 0xac, 0x74, 0xc3, 0x3b, 0x5d,
		0x2a, 0x83, 0x46, 0xb3, 0x3a, 0xac, 0xd8, 0x55, 0x68, 0x21, 0x57, 0xab, 0x6e, 0xd1, 0xd0, 0xfc,
		0xe2, 0xbe, 0x63, 0xd0, 0x6b, 0x79, 0x23, 0x40, 0x58, 0xd4, 0xe7, 0x73, 0x22, 0x67, 0x7f, 0x88,
		0x05, 0xbd, 0xdf, 0x7a, 0x65, 0x41, 0x90, 0x3a, 0x52, 0x83, 0x28, 0xae, 0xe9, 0x8e, 0x65, 0x82,
		0x0e, 0xdf, 0x98, 0x88, 0xe1, 0x86, 0x21, 0x3e, 0x1a, 0x87, 0x6d, 0x62, 0x7a, 0xf6, 0xaf, 0x2c,
		0xd5, 0xc5, 0x10, 0x2d, 0xa9, 0xda, 0x93, 0xa1, 0x9b, 0xc7, 0x35, 0xd4, 0x15, 0x78, 0x18, 0xd5,
		0x75, 0x6a, 0xd7, 0xdb, 0x12, 0x2a, 0x6a, 0xc8, 0x36, 0x53, 0x57, 0xa6, 0xf0, 0x13, 0x67, 0x43,
		0x79, 0xf0, 0x0e, 0x49, 0xb1, 0xec, 0xcd, 0xa4, 0x8a, 0x61, 0x06, 0xb9, 0xea, 0x53, 0xf2, 0x47,
		0x7d, 0xd6, 0xf8, 0x9d, 0x2e, 0xaa, 0x27, 0x35, 0x61, 0xce, 0x9b, 0x63, 0xbc, 0x07, 0x51, 0x5a,
		0xc2, 0x0d, 0x39, 0x42, 0xd2, 0x5e, 0x21, 0x20, 0x10, 0xa0, 0xe5, 0x08, 0xf7, 0x3d, 0x28, 0x04,
		0x99, 0x93, 0x97, 0xaf, 0xf9, 0x12, 0xc0, 0x01, 0x2d, 0xea, 0xf3, 0x98, 0x0b, 0x46, 0xc2, 0x26,
		0x93, 0x10, 0x69, 0x1d, 0x71, 0x8e, 0x33, 0x00, 0x5e, 0x80, 0x2f, 0x47, 0x0a, 0xcc, 0x94, 0x16,
		0xe7, 0x37, 0x45, 0xd0, 0x61, 0x79, 0x32, 0x86, 0x08, 0x2a, 0x5b, 0x55, 0xfe, 0xee, 0x52, 0x38,
		0xaa, 0x18, 0xe9, 0x39, 0x1a, 0x1e, 0xb8, 0x26, 0x6b, 0x3d, 0x4b, 0xa9
	},
	{ // region 4, $14d500
		0x06, 0x01, 0xf3, 0x39, 0xa0, 0x09, 0xa0, 0x21, 0xb0, 0x6f, 0x32, 0x8b, 0xfd, 0x89, 0x29, 0xa0,
		0x4a, 0x62, 0xed, 0xa1, 0x2d, 0xa4, 0x49, 0xf2, 0x10, 0x3c, 0x77, 0xa3, 0x84, 0x8d, 0xfa, 0xd1,
		0xc6, 0x57, 0xe2, 0x78, 0xef, 0xe9, 0xb6, 0xa1, 0x5a, 0xbd, 0x3f, 0x02, 0x0b, 0x28, 0xd6, 0x76,
		0xfc, 0x5b, 0x19, 0x9f, 0x21, 0x66, 0x4c, 0x2d, 0x45, 0x99, 0xde, 0xab, 0x46, 0xbd, 0xe9, 0x84,
		0xc4, 0xdc, 0xc7, 0x30, 0x70, 0xdd, 0x64, 0xea, 0xbc, 0x6b, 0xd3, 0xe6, 0x45, 0x3f, 0x07, 0x7e,
		0x50, 0xef, 0xb2, 0x84, 0x33, 0x3c, 0xcc, 0x3f, 0x39, 0x5b, 0xf5, 0x6d, 0x71, 0xc5, 0xdd, 0xf5,
		0xf9, 0xd0, 0xf7, 0x9c, 0xe6, 0xc7, 0xad, 0x1b, 0x29, 0xb9, 0x90, 0x08, 0x75, 0xc4, 0xc3, 0xef,
		0xa8, 0xfc, 0xab, 0x55, 0x7c, 0x21, 0x57, 0x97, 0x87, 0x4a, 0xcb, 0x0c, 0x56, 0x0a, 0x4f, 0xcb,
		0x52, 0x33, 0x87, 0x31, 0xf3, 0x43, 0x5b, 0x41, 0x90, 0xf8, 0xc0, 0xdd, 0x5a, 0xa4, 0x26, 0x2a,
		0x60, 0xa5, 0x6d, 0xda, 0xf2, 0x6a, 0xf0, 0xb3, 0xda, 0x25, 0x33, 0x87, 0x22, 0xe4, 0xac, 0xd3,
		0x96, 0xe0, 0x99, 0x3e, 0xfb, 0x14, 0x45, 0x17, 0x25, 0x56, 0xbe, 0xef, 0x8f, 0x8e, 0x3d, 0x1e,
		0xc7, 0x99, 0xa2, 0xa1, 0x50, 0xfe, 0xdf, 0xd4, 0xa1, 0x87, 0xf4, 0xd5, 0xde, 0xa6, 0x8c, 0x6d,
		0x6c, 0xde, 0x47, 0xbe, 0x59, 0x8f, 0xd4, 0x97, 0xc3, 0xf4, 0xda, 0xbb, 0xa6, 0x73, 0xa9, 0xcb,
		0xf2, 0x01, 0xb9, 0x90, 0x8f, 0xed, 0x60, 0x64, 0x40, 0x1c, 0xb6, 0xc9, 0xa5, 0x7c, 0x17, 0x52,
		0x6f, 0xdc, 0x6d, 0x08, 0x2a, 0x1a, 0xe6, 0x68, 0x3f, 0xd4, 0x42, 0x69
	},
	{ // region 5, $14bfb2
		0x7f, 0x41, 0xf3, 0x39, 0xa0, 0x09, 0xa1, 0x21, 0xb0, 0xa2, 0x4c, 0x23, 0x13, 0xe9, 0x25, 0x3d,
		0x0f, 0x72, 0x3a, 0x9d, 0xb5, 0x96, 0xd1, 0xda, 0x07, 0x29, 0x41, 0x9a, 0xad, 0x70, 0xba, 0x46,
		0x63, 0x2b, 0x7f, 0x3d, 0xbe, 0x40, 0xad, 0xd4, 0x4c, 0x73, 0x27, 0x58, 0xa7, 0x65, 0xdc, 0xd6,
		0xfd, 0xde, 0xb5, 0x6e, 0xd6, 0x6c, 0x75, 0x1a, 0x32, 0x45, 0xd5, 0xe3, 0x6a, 0x14, 0x6d, 0x80,
		0x84, 0x15, 0xaf, 0xcc, 0x7b, 0x61, 0x51, 0x82, 0x40, 0x53, 0x7f, 0x38, 0xa0, 0xd6, 0x8f, 0x61,
		0x79, 0x19, 0xe5, 0x99, 0x84, 0xd8, 0x78, 0x27, 0x3f, 0x16, 0x97, 0x78, 0x4f, 0x7b, 0x0c, 0xa6,
		0x37, 0xdb, 0xc6, 0x0c, 0x24, 0xb4, 0xc7, 0x94, 0x9d, 0x92, 0xd2, 0x3b, 0xd5, 0x11, 0x6f, 0x0a,
		0xdb, 0x76, 0x66, 0xe7, 0xcd, 0x18, 0x2b, 0x66, 0xd8, 0x41, 0x40, 0x58, 0xa2, 0x01, 0x1e, 0x6d,
		0x44, 0x75, 0xe7, 0x19, 0x4f, 0xb2, 0xe8, 0xc4, 0x96, 0x77, 0x62, 0x02, 0xc9, 0xdc, 0x59, 0xf3,
		0x43, 0x8d, 0xc8, 0xfe, 0x9e, 0x2a, 0xba, 0x32, 0x3b, 0x62, 0xe3, 0x92, 0x6e, 0xc2, 0x08, 0x4d,
		0x51, 0xcd, 0xf9, 0x3a, 0x3e, 0xc9, 0x50, 0x27, 0x21, 0x25, 0x97, 0xd7, 0x0e, 0xf8, 0x39, 0x38,
		0xf5, 0x86, 0x94, 0x93, 0xbf, 0xeb, 0x18, 0xa8, 0xfc, 0x24, 0xf5, 0xf9, 0x99, 0x20, 0x3d, 0xcd,
		0x2c, 0x94, 0x25, 0x79, 0x28, 0x77, 0x8f, 0x2f, 0x10, 0x69, 0x86, 0x30, 0x43, 0x01, 0xd7, 0x9a,
		0x17, 0xe3, 0x47, 0x37, 0xbd, 0x62, 0x75, 0x42, 0x78, 0xf4, 0x2b, 0x57, 0x4c, 0x0a, 0xdb, 0x53,
		0x4d, 0xa1, 0x0a, 0xd6, 0x3a, 0x16, 0x15, 0xaa, 0x2c, 0x6c, 0x39, 0x42
	},
	{ // region 6, $14cd82
		0x12, 0x09, 0xf3, 0x29, 0xa0, 0x09, 0xa0, 0x21, 0xb0, 0xd5, 0x66, 0xa1, 0x28, 0x4a, 0x21, 0xc0,
		0xd3, 0x9b, 0x86, 0x80, 0x57, 0x6f, 0x41, 0xc2, 0xe4, 0x2f, 0x0b, 0x91, 0xbd, 0x3a, 0x7a, 0xba,
		0x00, 0xe5, 0x35, 0x02, 0x74, 0x7d, 0x8b, 0x21, 0x57, 0x10, 0x0f, 0xae, 0x44, 0xbb, 0xe2, 0x37,
		0x18, 0x7b, 0x52, 0x3d, 0x8c, 0x59, 0x9e, 0x20, 0x1f, 0x0a, 0xcc, 0x1c, 0x8e, 0x6a, 0xd7, 0x95,
		0x2b, 0x34, 0xb0, 0x82, 0x6d, 0xfd, 0x25, 0x33, 0xaa, 0x3b, 0x2b, 0x70, 0x15, 0x87, 0x31, 0x5d,
		0xbb, 0x29, 0x19, 0x95, 0xd5, 0x8e, 0x24, 0x28, 0x5e, 0xd0, 0x20, 0x83, 0x46, 0x4a, 0x21, 0x70,
		0x5b, 0xcd, 0xae, 0x7b, 0x61, 0xa1, 0xfa, 0xf4, 0x2b, 0x84, 0x15, 0x6e, 0x36, 0x5d, 0x1b, 0x24,
		0x0f, 0x09, 0x3a, 0x61, 0x38, 0x0f, 0x18, 0x35, 0x11, 0x38, 0xb4, 0xbd, 0xee, 0xf7, 0xec, 0x0f,
		0x1d, 0xb7, 0x48, 0x01, 0xaa, 0x09, 0x8f, 0x61, 0xb5, 0x0f, 0x1d, 0x26, 0x39, 0x2e, 0x8c, 0xd6,
		0x26, 0x5c, 0x3d, 0x23, 0x63, 0xe9, 0x6b, 0x97, 0xb4, 0x9f, 0x7b, 0xb6, 0xba, 0xa0, 0x7c, 0xc6,
		0x25, 0xa1, 0x73, 0x36, 0x67, 0x7f, 0x74, 0x1e, 0x1d, 0xda, 0x70, 0xbf, 0xa5, 0x63, 0x35, 0x39,
		0x24, 0x8c, 0x9f, 0x85, 0x16, 0xd8, 0x50, 0x95, 0x71, 0xc0, 0xf6, 0x1e, 0x6d, 0x80, 0xed, 0x15,
		0xeb, 0x63, 0xe9, 0x1b, 0xf6, 0x78, 0x31, 0xc6, 0x5c, 0xdd, 0x19, 0xbd, 0xdf, 0xa7, 0xec, 0x50,
		0x22, 0xad, 0xbb, 0xf6, 0xeb, 0xd6, 0xa3, 0x20, 0xc9, 0xe6, 0x9f, 0xcb, 0xf2, 0x97, 0xb9, 0x54,
		0x12, 0x66, 0xa6, 0xbe, 0x4a, 0x12, 0x43, 0xec, 0x00, 0xea, 0x49, 0x02
	},
	{ // region 7, $14ce76
		0xa4, 0x49, 0xf3, 0x29, 0xa0, 0x09, 0xa1, 0x21, 0xb0, 0xef, 0x80, 0x20, 0x3d, 0xaa, 0x36, 0x5d,
		0x98, 0xc4, 0xd2, 0x63, 0xdf, 0x61, 0xb0, 0xc3, 0xc2, 0x35, 0xd4, 0x88, 0xe6, 0x1d, 0x3a, 0x2f,
		0x9c, 0xb9, 0xd1, 0xc6, 0x43, 0xba, 0x69, 0x6d, 0x49, 0xac, 0xdd, 0x05, 0xe0, 0xf8, 0xe8, 0x97,
		0x19, 0x18, 0x08, 0x0c, 0x42, 0x46, 0xc7, 0x0d, 0x25, 0xce, 0xc3, 0x54, 0xb2, 0xd9, 0x42, 0x91,
		0xea, 0x53, 0x98, 0x38, 0x78, 0x81, 0x12, 0xca, 0x15, 0x23, 0xbd, 0xc1, 0x70, 0x1f, 0xd2, 0x40,
		0xfd, 0x39, 0x33, 0xaa, 0x27, 0x2b, 0xe8, 0x10, 0x7d, 0xa4, 0xa8, 0x8e, 0x3d, 0x00, 0x4f, 0x3a,
		0x7f, 0xd8, 0x96, 0xea, 0x9e, 0x8e, 0x15, 0x6e, 0x9f, 0x76, 0x57, 0xba, 0x7d, 0xc2, 0xdf, 0x57,
		0x42, 0x82, 0xf4, 0xda, 0x89, 0x06, 0x05, 0x04, 0x62, 0x2f, 0x29, 0x23, 0x54, 0xd5, 0xbb, 0x97,
		0xf5, 0xf9, 0xc1, 0xcf, 0xec, 0x5f, 0x1d, 0xfd, 0xbb, 0xa6, 0xd7, 0x4a, 0xa8, 0x66, 0xbf, 0xb9,
		0x09, 0x44, 0xb1, 0x60, 0x28, 0xa9, 0x35, 0x16, 0x15, 0xf5, 0x13, 0xc1, 0x07, 0x7e, 0xd7, 0x40,
		0xdf, 0x8e, 0xd3, 0x32, 0xa9, 0x35, 0x98, 0x15, 0x32, 0xa9, 0x49, 0xc0, 0x24, 0xb4, 0x4a, 0x53,
		0x6b, 0x79, 0xaa, 0x77, 0x6c, 0xc5, 0x88, 0x69, 0xe5, 0x5d, 0xde, 0x42, 0x28, 0xf9, 0xb7, 0x5c,
		0xab, 0x19, 0xc7, 0xbc, 0xc5, 0x60, 0xeb, 0x5e, 0xa8, 0x52, 0xc4, 0x32, 0x7c, 0x35, 0x02, 0x06,
		0x46, 0x77, 0x30, 0xb6, 0x33, 0x4b, 0xb8, 0xfd, 0x02, 0xd8, 0x14, 0x40, 0x99, 0x25, 0x7e, 0x55,
		0xd6, 0x44, 0x43, 0x8d, 0x73, 0x0e, 0x71, 0x48, 0xd3, 0x82, 0x40, 0xda
	},
	{ 0, }, // unused region 08
	{ 0, }, // unused region 09
	{ 0, }, // unused region 0a
	{ 0, }, // unused region 0b
	{ 0, }, // unused region 0c
	{ 0, }, // unused region 0d
	{ 0, }, // unused region 0e
	{ 0, }, // unused region 0f
	{ 0, }, // unused region 10
	{ 0, }, // unused region 11
	{ 0, }, // unused region 12
	{ 0, }, // unused region 13
	{ 0, }, // unused region 14
	{ 0, }, // unused region 15
	{ // region 16, $178772
		0x5e, 0x09, 0xb3, 0x39, 0x60, 0x71, 0x71, 0x53, 0x11, 0xe5, 0x26, 0x34, 0x4c, 0x8c, 0x90, 0xee,
		0xed, 0xb5, 0x05, 0x95, 0x9e, 0x6b, 0xdd, 0x87, 0x0e, 0x7b, 0xed, 0x33, 0xaf, 0xc2, 0x62, 0x98,
		0xec, 0xc8, 0x2c, 0x2b, 0x57, 0x3d, 0x00, 0xbd, 0x12, 0xac, 0xba, 0x64, 0x81, 0x99, 0x16, 0x29,
		0xb4, 0x63, 0xa8, 0xd9, 0xc9, 0x5f, 0xfe, 0x21, 0xbb, 0xbf, 0x9b, 0xd1, 0x7b, 0x93, 0xc4, 0x82,
		0xef, 0x2b, 0xe8, 0xa6, 0xdc, 0x68, 0x3a, 0xd9, 0xc9, 0x23, 0xc7, 0x7b, 0x98, 0x5b, 0xe1, 0xc7,
		0xa3, 0xd4, 0x51, 0x0a, 0x86, 0x30, 0x20, 0x51, 0x6e, 0x04, 0x1c, 0xd4, 0xfb, 0xf5, 0x22, 0x8f,
		0x16, 0x6f, 0xb9, 0x59, 0x30, 0xcf, 0xab, 0x32, 0x1d, 0x6c, 0x84, 0xab, 0x23, 0x90, 0x94, 0xb1,
		0xe7, 0x4b, 0x6d, 0xc1, 0x84, 0xba, 0x32, 0x68, 0xa3, 0xf2, 0x47, 0x28, 0xe5, 0xcb, 0xbb, 0x47,
		0x14, 0x2c, 0xad, 0x4d, 0xa1, 0xd7, 0x18, 0x53, 0xf7, 0x6f, 0x05, 0x81, 0x8f, 0xbb, 0x29, 0xdc,
		0xbd, 0x17, 0x61, 0x92, 0x9b, 0x1d, 0x4e, 0x7a, 0x83, 0x14, 0x9f, 0x7b, 0x7a, 0x6a, 0xe1, 0x27,
		0x62, 0x52, 0x7e, 0x82, 0x45, 0xda, 0xed, 0xf1, 0x0a, 0x3b, 0x6c, 0x02, 0x5b, 0x6e, 0x45, 0x4e,
		0xf2, 0x65, 0x87, 0x1d, 0x80, 0xed, 0x6a, 0xc3, 0x77, 0xcb, 0xe8, 0x8d, 0x5a, 0xb8, 0xda, 0x89,
		0x88, 0x4b, 0x27, 0xd5, 0x57, 0x29, 0x91, 0x86, 0x12, 0xbb, 0xd3, 0x8c, 0xc7, 0x49, 0x84, 0x9c,
		0x96, 0x59, 0x30, 0x93, 0x92, 0xeb, 0x59, 0x2b, 0x93, 0x5b, 0x5f, 0xf9, 0x67, 0xac, 0x97, 0x8c,
		0x04, 0xda, 0x1b, 0x65, 0xd7, 0xef, 0x44, 0xca, 0xc4, 0x87, 0x18, 0x2b
	},
	{ // region 17, $178a36
		0xd7, 0x49, 0xb3, 0x39, 0x60, 0x71, 0x70, 0x53, 0x11, 0x00, 0x27, 0xb2, 0x61, 0xd3, 0x8c, 0x8b,
		0xb2, 0xde, 0x6a, 0x78, 0x40, 0x5d, 0x4d, 0x88, 0xeb, 0x81, 0xd0, 0x2a, 0xbf, 0x8c, 0x22, 0x0d,
		0x89, 0x83, 0xc8, 0xef, 0x0d, 0x7a, 0xf6, 0xf0, 0x1d, 0x49, 0xa2, 0xd3, 0x1e, 0xef, 0x1c, 0xa2,
		0xce, 0x00, 0x5e, 0xa8, 0x7f, 0x4c, 0x41, 0x27, 0xa8, 0x6b, 0x92, 0x0a, 0xb8, 0x03, 0x2f, 0x7e,
		0xaf, 0x4a, 0xd0, 0x5c, 0xce, 0xeb, 0x0e, 0x8a, 0x4d, 0x0b, 0x73, 0xb3, 0xf3, 0x0c, 0x83, 0xaa,
		0xe5, 0xe4, 0x84, 0x06, 0xd7, 0xcc, 0xcb, 0x52, 0x8d, 0xbe, 0xa4, 0xdf, 0xd9, 0xab, 0x50, 0x59,
		0x53, 0x61, 0xa1, 0xc8, 0x6d, 0xbc, 0xde, 0xab, 0xaa, 0x5e, 0xc6, 0xf7, 0x83, 0xdc, 0x40, 0xcb,
		0x1b, 0xdd, 0x28, 0x3b, 0xee, 0xb1, 0x1f, 0x37, 0xdb, 0xe9, 0xbb, 0x74, 0x4b, 0xc2, 0x8a, 0xe8,
		0xec, 0x6e, 0x0e, 0x35, 0xe3, 0x2e, 0xbe, 0xef, 0xfd, 0x07, 0xbf, 0x8c, 0xfe, 0xf3, 0x5c, 0xbf,
		0x87, 0xe5, 0xbc, 0xcf, 0x60, 0xdc, 0x18, 0xf8, 0xfc, 0x51, 0x50, 0x86, 0xc6, 0x48, 0x3d, 0xb9,
		0x1d, 0x26, 0xf7, 0x7e, 0x87, 0x90, 0x12, 0xe8, 0x06, 0x0a, 0x45, 0xe9, 0xd9, 0xd8, 0x41, 0x68,
		0x21, 0x52, 0x92, 0x0f, 0xd6, 0xda, 0xa2, 0x97, 0xeb, 0x68, 0xd0, 0xb1, 0x15, 0x19, 0x8b, 0xd0,
		0x48, 0x1a, 0xeb, 0x90, 0x3f, 0x2a, 0x33, 0x1e, 0x5e, 0x30, 0x66, 0x01, 0x64, 0xef, 0x99, 0x52,
		0xba, 0x23, 0xbd, 0x53, 0xc0, 0x60, 0x87, 0x09, 0xcb, 0x4d, 0xd3, 0x87, 0x0e, 0x3a, 0x5c, 0x8d,
		0xc8, 0xb8, 0xb7, 0x34, 0x01, 0xeb, 0x72, 0x0d, 0xb1, 0x1f, 0x0f, 0xea
	},
	{ // region 18, $17dac4
		0x6a, 0x13, 0xb3, 0x09, 0x60, 0x79, 0x61, 0x53, 0x11, 0x33, 0x41, 0x31, 0x76, 0x34, 0x88, 0x0f,
		0x77, 0x08, 0xb6, 0x74, 0xc8, 0x36, 0xbc, 0x70, 0xe2, 0x87, 0x9a, 0x21, 0xe8, 0x56, 0xe1, 0x9a,
		0x26, 0x57, 0x7e, 0x9b, 0xdb, 0xb7, 0xd4, 0x3d, 0x0f, 0xfe, 0x8a, 0x2a, 0xba, 0x2d, 0x22, 0x03,
		0xcf, 0x9c, 0xfa, 0x77, 0x35, 0x39, 0x6a, 0x14, 0xae, 0x30, 0x89, 0x42, 0xdc, 0x59, 0xb2, 0x93,
		0x6f, 0x82, 0xd1, 0x12, 0xd9, 0x88, 0xfa, 0x3b, 0xb7, 0x0c, 0x1f, 0x05, 0x68, 0xa3, 0x0c, 0xa6,
		0x0f, 0xf4, 0x9e, 0x1b, 0x29, 0x82, 0x77, 0x3a, 0xac, 0x92, 0x2d, 0x04, 0xd0, 0x61, 0x65, 0x0a,
		0x77, 0x6c, 0x89, 0x38, 0xaa, 0xa9, 0xf8, 0x0c, 0x1f, 0x37, 0x09, 0x2b, 0xca, 0x29, 0x05, 0xe5,
		0x4e, 0x57, 0xfb, 0xcd, 0x40, 0xa8, 0x0c, 0x06, 0x2d, 0xe0, 0x30, 0xd9, 0x97, 0xb9, 0x59, 0x8a,
		0xde, 0xc9, 0x87, 0x1d, 0x3f, 0x84, 0x4c, 0x73, 0x04, 0x85, 0x61, 0xb0, 0x6e, 0x2c, 0x8f, 0xa2,
		0x6a, 0xcd, 0x31, 0xf3, 0x25, 0x83, 0xe1, 0x5e, 0x5d, 0xa7, 0xe7, 0xaa, 0x13, 0x26, 0xb1, 0x33,
		0xf0, 0x13, 0x58, 0x7a, 0xb0, 0x46, 0x1d, 0xdf, 0x02, 0xbf, 0x1e, 0xd1, 0x71, 0x43, 0x56, 0x82,
		0x4f, 0x58, 0x9d, 0x01, 0x2d, 0xc7, 0xda, 0x6b, 0x47, 0x05, 0xd1, 0xd5, 0xe8, 0x92, 0x3c, 0x18,
		0x21, 0xcf, 0xc9, 0x32, 0x0e, 0x12, 0xed, 0xb5, 0xaa, 0xa4, 0x12, 0x75, 0x01, 0x7d, 0xc7, 0x21,
		0xde, 0xec, 0x32, 0x13, 0xee, 0xd4, 0x9c, 0xe6, 0x04, 0x3f, 0x48, 0xfb, 0xb4, 0xc7, 0x21, 0x8e,
		0x8d, 0x7d, 0x54, 0x03, 0x11, 0xe7, 0xb9, 0x4f, 0x85, 0xb6, 0x1f, 0xaa
	},
	{ // region 19, $178eee
		0xe3, 0x53, 0xb3, 0x09, 0x60, 0x79, 0x60, 0x53, 0x11, 0x66, 0x5b, 0xc8, 0x8b, 0x94, 0x84, 0xab,
		0x3c, 0x18, 0x03, 0x57, 0x6a, 0x0f, 0x45, 0x58, 0xc0, 0x74, 0x64, 0x18, 0xf8, 0x39, 0xa1, 0x0f,
		0xc2, 0x2b, 0x1b, 0x60, 0xaa, 0x0e, 0xb2, 0x89, 0x01, 0x9b, 0x72, 0x80, 0x57, 0x83, 0x28, 0x63,
		0xe9, 0x39, 0x97, 0x46, 0xea, 0x3f, 0x93, 0x01, 0x9b, 0xf4, 0x80, 0x93, 0x01, 0xaf, 0x1d, 0x8f,
		0x16, 0xa1, 0xb9, 0xc7, 0xe4, 0x0c, 0xe7, 0xd2, 0x3b, 0xf3, 0xca, 0x3d, 0xc3, 0x54, 0xad, 0x89,
		0x51, 0x1e, 0xd1, 0x17, 0x7a, 0x1f, 0x23, 0x22, 0xcb, 0x4d, 0xce, 0x0f, 0xae, 0x30, 0x93, 0xd3,
		0x9b, 0x77, 0x71, 0xa7, 0xe7, 0x96, 0x2c, 0x85, 0xac, 0x29, 0x4b, 0x5e, 0x2b, 0x75, 0xb0, 0x00,
		0x81, 0xe9, 0xb6, 0x47, 0xaa, 0x9f, 0xdf, 0xd4, 0x7e, 0xd7, 0xa4, 0x3f, 0xe3, 0xb0, 0x41, 0x2c,
		0xb7, 0x0c, 0xe7, 0xeb, 0x9a, 0xda, 0xd9, 0x10, 0x23, 0x1d, 0x1c, 0xd4, 0xdd, 0x7d, 0xc2, 0x6c,
		0x4d, 0x9c, 0xa5, 0x18, 0xd0, 0x43, 0xab, 0xdc, 0xbd, 0xe4, 0x7f, 0xb5, 0x5f, 0x04, 0x0d, 0xac,
		0xab, 0xe6, 0xb8, 0x76, 0xf2, 0x15, 0x41, 0xef, 0x17, 0x8e, 0xf6, 0xb9, 0xef, 0x94, 0x52, 0x83,
		0x96, 0x45, 0x8f, 0xf2, 0x9c, 0xb4, 0x13, 0x3f, 0xbb, 0xa1, 0xd2, 0xf9, 0xa3, 0xf2, 0x06, 0x78,
		0xe0, 0x9e, 0xa7, 0xd3, 0xdc, 0x13, 0x8f, 0x4d, 0xf6, 0x19, 0xbd, 0x03, 0x9d, 0x24, 0xdc, 0xd6,
		0xe9, 0xcf, 0xa6, 0xd2, 0x1d, 0x49, 0xca, 0xc4, 0x55, 0x18, 0xbc, 0x70, 0x5b, 0x55, 0xfe, 0x8f,
		0x6b, 0x42, 0xf0, 0xd1, 0x21, 0xe3, 0xe7, 0x91, 0x59, 0x4e, 0x16, 0x83
	},
	{ 0, }, // unused region 1a
	{ 0, }, // unused region 1b
	{ 0, }, // unused region 1c
	{ 0, }, // unused region 1d
	{ 0, }, // unused region 1e
	{ 0, }, // unused region 1f
	{ // region 20, $17a322
		0xb3, 0x10, 0xf3, 0x0b, 0xe0, 0x71, 0x60, 0x53, 0x11, 0x9a, 0x12, 0x70, 0x1f, 0x1e, 0x81, 0xda,
		0x9d, 0x1f, 0x4b, 0xd6, 0x71, 0x48, 0x83, 0xe1, 0x04, 0x6c, 0x1b, 0xf1, 0xcd, 0x09, 0xdf, 0x3e,
		0x0b, 0xaa, 0x95, 0xc1, 0x07, 0xec, 0x0f, 0x54, 0xd0, 0x16, 0xb0, 0xdc, 0x86, 0x7b, 0x52, 0x38,
		0x3c, 0x68, 0x2b, 0xed, 0xe2, 0xeb, 0xb3, 0xc6, 0x48, 0x24, 0x41, 0x36, 0x17, 0x25, 0x1f, 0xa5,
		0x22, 0xc6, 0x5c, 0xa6, 0x19, 0xef, 0x17, 0x5c, 0x56, 0x4b, 0x4a, 0x2b, 0x75, 0xab, 0xe6, 0x22,
		0xd5, 0xc0, 0xd3, 0x46, 0xcc, 0xe4, 0xd4, 0xc4, 0x8c, 0x9a, 0x8a, 0x75, 0x24, 0x73, 0xa4, 0x26,
		0xca, 0x79, 0xaf, 0xb3, 0x94, 0x2a, 0x15, 0xbe, 0x40, 0x7b, 0x4d, 0xf6, 0xb4, 0xa4, 0x7b, 0xcf,
		0xce, 0xa0, 0x1d, 0xcb, 0x2f, 0x60, 0x28, 0x63, 0x85, 0x98, 0xd3, 0xd2, 0x45, 0x3f, 0x02, 0x65,
		0xd7, 0xf4, 0xbc, 0x2a, 0xe7, 0x50, 0xd1, 0x3f, 0x7f, 0xf6, 0x05, 0xb8, 0xe9, 0x39, 0x10, 0x6e,
		0x68, 0xa8, 0x89, 0x60, 0x00, 0x68, 0xfd, 0x20, 0xc4, 0xdc, 0xef, 0x67, 0x75, 0xfb, 0xbe, 0xfe,
		0x2b, 0x16, 0xa6, 0x5a, 0x77, 0x0d, 0x0c, 0xe2, 0x2d, 0xd1, 0xe4, 0x11, 0xc9, 0x4b, 0x81, 0x3a,
		0x0c, 0x24, 0xaa, 0x77, 0x2b, 0x2f, 0x83, 0x23, 0xd1, 0xe9, 0xa7, 0x29, 0x0a, 0xf9, 0x26, 0x9d,
		0x51, 0xc8, 0x6d, 0x71, 0x9d, 0xce, 0x46, 0x72, 0x26, 0x48, 0x3d, 0x64, 0xe5, 0x67, 0xbb, 0x1a,
		0xb4, 0x6d, 0x21, 0x11, 0x79, 0x78, 0xc2, 0xd5, 0x11, 0x6a, 0xd2, 0xea, 0x03, 0x4d, 0x92, 0xaf,
		0x18, 0xd5, 0x07, 0x79, 0xaa, 0xf9, 0x44, 0x93, 0x6f, 0x41, 0x22, 0x0d
	},
	{ // region 21, $17b3b4
		0x2d, 0x50, 0xf3, 0x0b, 0xe0, 0x71, 0x61, 0x53, 0x11, 0xb4, 0x2c, 0xee, 0x34, 0x7e, 0x7d, 0x5e,
		0x62, 0x48, 0x97, 0xd2, 0xf9, 0x3a, 0xf2, 0xc9, 0xfa, 0x59, 0xe4, 0xe8, 0xf6, 0xd2, 0x9f, 0xb2,
		0xa7, 0x7e, 0x32, 0x86, 0xbc, 0x43, 0xec, 0xa0, 0xc2, 0xcb, 0x98, 0x33, 0x23, 0xd1, 0x58, 0x98,
		0x56, 0x05, 0xc7, 0xbc, 0x98, 0xd8, 0xdc, 0xb3, 0x35, 0xe8, 0x51, 0x6e, 0x3b, 0x7b, 0x89, 0xba,
		0xe1, 0xe5, 0x44, 0x5c, 0x24, 0x73, 0x04, 0x0d, 0xd9, 0x33, 0xf5, 0x63, 0xe9, 0x5c, 0x88, 0x05,
		0x18, 0xd0, 0x07, 0x5b, 0x1e, 0x81, 0x80, 0xac, 0x92, 0x6e, 0x13, 0x80, 0x1b, 0x29, 0xd2, 0xef,
		0x08, 0x84, 0x97, 0x23, 0xd1, 0x17, 0x2f, 0x38, 0xb4, 0x6d, 0x8f, 0x2a, 0x15, 0xf0, 0x40, 0xe9,
		0x02, 0x33, 0xd7, 0x5e, 0x99, 0x57, 0x15, 0x32, 0xbd, 0x8f, 0x48, 0x38, 0x91, 0x36, 0xe9, 0x07,
		0xc9, 0x37, 0x1d, 0x12, 0x2a, 0xbf, 0x5f, 0xdb, 0x85, 0x75, 0xbf, 0xdc, 0x59, 0x8a, 0x43, 0x51,
		0x4b, 0x77, 0xfd, 0x84, 0xc4, 0x28, 0xc7, 0x85, 0x25, 0x1a, 0x87, 0x8b, 0xc1, 0xd9, 0x1a, 0x78,
		0xe5, 0x03, 0x20, 0x56, 0xa0, 0xc2, 0x17, 0xf2, 0x29, 0xa0, 0xbd, 0xf8, 0x61, 0x9c, 0x7d, 0x54,
		0x3a, 0x11, 0xb5, 0x69, 0x9a, 0x1c, 0xbb, 0xf6, 0x2d, 0x86, 0xa8, 0x4d, 0xdd, 0x5a, 0xd6, 0xe4,
		0x11, 0x7e, 0x4b, 0x13, 0x6c, 0xb6, 0x01, 0x0a, 0x72, 0xbc, 0xe8, 0xf1, 0x82, 0x0e, 0xd0, 0xcf,
		0xbf, 0x50, 0x95, 0xb7, 0xa7, 0xec, 0xd7, 0xb3, 0x49, 0x5c, 0x47, 0x5f, 0xa9, 0xda, 0x70, 0xb0,
		0xdc, 0x9a, 0xa3, 0x48, 0xd3, 0xf5, 0x72, 0xd5, 0x43, 0xd8, 0x19, 0xcc
	}
};

static UINT16 *sharedprotram;

static UINT32 kb_regs[0x100];
static UINT16 kb_prot_hold;
static UINT16 kb_prot_hilo;
static UINT32 kb_ptr;
static UINT8 kb_region;
static UINT8 kb_cmd;
static UINT8 kb_reg;
static UINT8 kb_swap;
static UINT8 kb_cmd3;
static UINT8 olds_bs;
static UINT32 kb_prot_hilo_select;
static UINT32 kb_game_id;

static void IGS022_do_dma(UINT16 src, UINT16 dst, UINT16 size, UINT16 mode)
{
	UINT16 param = mode >> 8;

	bprintf (0, _T("src: %4.4x, dst: %4.4x, size: %4.4x, mode: %4.4x\n"), src,dst,size,mode);

	mode &= 0x7;  // what are the other bits?

	if ((mode == 0) || (mode == 1) || (mode == 2) || (mode == 3)  || (mode == 4))
	{
		UINT16 *PROTROM = (UINT16*)PGMProtROM;
		for (INT32 x = 0; x < size; x++)
		{
			UINT16 dat2 = BURN_ENDIAN_SWAP_INT16(PROTROM[src + x]);

			UINT8 extraoffset = param&0xff;
			UINT8* dectable = PGMProtROM;
			UINT8 taboff = ((x*2)+extraoffset) & 0xff;
			UINT16 extraxor = ((dectable[taboff+0]) << 8) | (dectable[taboff+1] << 0);

			dat2 = ((dat2 & 0x00ff)<<8) | ((dat2 & 0xff00)>>8);

			if (mode==4)
			{
				extraxor = 0;
				if ((x & 0x003) == 0x000) extraxor |= 0x0049; // 'I'
				if ((x & 0x003) == 0x001) extraxor |= 0x0047; // 'G'
				if ((x & 0x003) == 0x002) extraxor |= 0x0053; // 'S'
				if ((x & 0x003) == 0x003) extraxor |= 0x0020; // ' '
				if ((x & 0x300) == 0x000) extraxor |= 0x4900; // 'I'
				if ((x & 0x300) == 0x100) extraxor |= 0x4700; // 'G'
				if ((x & 0x300) == 0x200) extraxor |= 0x5300; // 'S'
				if ((x & 0x300) == 0x300) extraxor |= 0x2000; // ' '

			//	extraxor = (extraxor << 8) | (extraxor >> 8); // 
			}

			//  mode==0 plain
			if (mode==3) dat2 ^= extraxor;
			if (mode==2) dat2 += extraxor;
			if (mode==1) dat2 -= extraxor;

			if (mode==4)
			{
				dat2 -= extraxor;
			}

			sharedprotram[dst + x] = BURN_ENDIAN_SWAP_INT16(dat2);
		}
	}
	else if (mode == 5)
	{
		UINT16 *PROTROM = (UINT16*)PGMProtROM;

		for (INT32 x = 0; x < size; x++)
		{
			UINT16 dat2 = BURN_ENDIAN_SWAP_INT16(PROTROM[src + x]);

			sharedprotram[dst + x] = dat2;
		}
	}
	else if (mode == 6)
	{
		UINT16 *PROTROM = (UINT16*)PGMProtROM;
		for (INT32 x = 0; x < size; x++)
		{
			UINT16 dat2 = PROTROM[src + x];

			dat2 = ((dat2 & 0xf000) >> 12)|
					((dat2 & 0x0f00) >> 4)|
					((dat2 & 0x00f0) << 4)|
					((dat2 & 0x000f) << 12);

			sharedprotram[dst + x] = dat2;
		}
	}
}

static void IGS022_reset()
{
	INT32 i;
	UINT16 *PROTROM = (UINT16*)PGMProtROM;
	UINT16 tmp;

	// fill ram with A5 patern
	for (i = 0; i < 0x4000/2; i++)
		sharedprotram[i] = BURN_ENDIAN_SWAP_INT16(0xa55a);

	// the auto-dma
	UINT16 src = BURN_ENDIAN_SWAP_INT16(PROTROM[0x100 / 2]);
	UINT32 dst = BURN_ENDIAN_SWAP_INT16(PROTROM[0x102 / 2]);
	UINT16 size = BURN_ENDIAN_SWAP_INT16(PROTROM[0x104/ 2]);
	UINT16 mode = BURN_ENDIAN_SWAP_INT16(PROTROM[0x106 / 2]);

	src = ((src & 0xff00) >> 8) | ((src & 0x00ff) << 8);
	dst = ((dst & 0xff00) >> 8) | ((dst & 0x00ff) << 8);
	size = ((size & 0xff00) >> 8) | ((size & 0x00ff) << 8);
	mode &= 0xff;

	src >>= 1;

	IGS022_do_dma(src,dst,size,mode);

	// there is also a version ID? (or is it some kind of checksum) that is stored in the data rom, and gets copied..
	// Dragon World 3 checks it
	tmp = BURN_ENDIAN_SWAP_INT16(PROTROM[0x114/2]);
	tmp = ((tmp & 0xff00) >> 8) | ((tmp & 0x00ff) << 8);
	sharedprotram[0x2a2/2] = BURN_ENDIAN_SWAP_INT16(tmp);
}

static void IGS022_handle_command()
{
	UINT16 cmd = sharedprotram[0x200/2];

	if (cmd == 0x6d)    // Store values to asic ram
	{
		UINT32 p1 = (sharedprotram[0x298/2] << 16) | sharedprotram[0x29a/2];
		UINT32 p2 = (sharedprotram[0x29c/2] << 16) | sharedprotram[0x29e/2];

		if ((p2 & 0xffff) == 0x9)   // Set value
		{
			INT32 reg = (p2 >> 16) & 0xffff;

			if (reg & 0x300) { // 300?? killbld expects 0x200, drgw3 expects 0x100?
				kb_regs[reg & 0xff] = p1;
			}
		}

		if ((p2 & 0xffff) == 0x6)   // Add value
		{
			INT32 src1 = (p1 >> 16) & 0xff;
			INT32 src2 = (p1 >> 0) & 0xff;
			INT32 dst = (p2 >> 16) & 0xff;

			kb_regs[dst] = kb_regs[src2] - kb_regs[src1];
		}

		if ((p2 & 0xffff) == 0x1)   // Add Imm?
		{
			INT32 reg = (p2 >> 16) & 0xff;
			INT32 imm = (p1 >> 0) & 0xffff;

			kb_regs[reg] += imm;
		}

		if ((p2 & 0xffff) == 0xa)   // Get value
		{
			INT32 reg = (p1 >> 16) & 0xFF;

			sharedprotram[0x29c/2] = (kb_regs[reg] >> 16) & 0xffff;
			sharedprotram[0x29e/2] = kb_regs[reg] & 0xffff;
		}

		sharedprotram[0x202 / 2] = 0x7c;  // this mode complete?
	}

	// Is this actually what this is suppose to do? Complete guess.
	if (cmd == 0x12) // copy??
	{
		sharedprotram[0x28c / 2] = sharedprotram[0x288 / 2];
		sharedprotram[0x28e / 2] = sharedprotram[0x28a / 2];

		sharedprotram[0x202 / 2] = 0x23;  // this mode complete?
	}

	// what do these do? write the completion byte for now...
	if (cmd == 0x45) sharedprotram[0x202 / 2] = 0x56;
	if (cmd == 0x5a) sharedprotram[0x202 / 2] = 0x4b;
	if (cmd == 0x2d) sharedprotram[0x202 / 2] = 0x3c;

	if (cmd == 0x4f) // memcpy with encryption / scrambling
	{
		UINT16 src  = sharedprotram[0x290 / 2] >> 1;
		UINT32 dst  = sharedprotram[0x292 / 2];
		UINT16 size = sharedprotram[0x294 / 2];
		UINT16 mode = sharedprotram[0x296 / 2];

		IGS022_do_dma(src,dst,size,mode);

		sharedprotram[0x202 / 2] = 0x5e;  // this mode complete?
	}
}

static UINT32 olds_prot_addr(UINT16 addr)
{
	switch (addr & 0xff)
	{
		case 0x0:
		case 0x5:
		case 0xa: return 0x402a00 + ((addr >> 8) << 2);
		case 0x2:
		case 0x8: return 0x402e00 + ((addr >> 8) << 2);
		case 0x1: return 0x40307e;
		case 0x3: return 0x403090;
		case 0x4: return 0x40309a;
		case 0x6: return 0x4030a4;
		case 0x7: return 0x403000;
		case 0x9: return 0x40306e;
		case 0xb: return 0x403044;
	}

	return 0;
}

static UINT32 olds_read_reg(UINT16 addr)
{
	UINT32 protaddr = (olds_prot_addr(addr) - 0x400000) / 2;
	return sharedprotram[protaddr] << 16 | sharedprotram[protaddr + 1];
}

static void olds_write_reg( UINT16 addr, UINT32 val )
{
	sharedprotram[((olds_prot_addr(addr) - 0x400000) / 2) + 0] = val >> 16;
	sharedprotram[((olds_prot_addr(addr) - 0x400000) / 2) + 1] = val & 0xffff;
}

static void IGS028_do_dma(UINT16 src, UINT16 dst, UINT16 size, UINT16 mode)
{
	UINT16 param = mode >> 8;
	UINT16 *PROTROM = (UINT16*)(PGMUSER0 + 0x10000);

	mode &= 0x0f;

	switch (mode & 0x7)
	{
		case 0x00: // -= encryption
		case 0x01: // swap nibbles
		case 0x02: // ^= encryption
		case 0x03: // unused?
		case 0x04: // unused?
		case 0x05: // swap bytes
		case 0x06: // += encryption (correct?)
		case 0x07: // unused?
		{
			UINT8 extraoffset = param & 0xff;
			UINT8 *dectable = (UINT8 *)(PGMProtROM + (0x100 / 2));

			for (INT32 x = 0; x < size; x++)
			{
				UINT16 dat2 = PROTROM[src + x];

				int taboff = ((x*2)+extraoffset) & 0xff; // must allow for overflow in instances of odd offsets
				unsigned short extraxor = ((dectable[taboff + 0]) << 0) | (dectable[taboff + 1] << 8);

				if (mode==0) dat2 -= extraxor;
				else if (mode==1) dat2  = ((dat2 & 0xf0f0) >> 4)|((dat2 & 0x0f0f) << 4);
				else if (mode==2) dat2 ^= extraxor;
				else if (mode==5) dat2  = ((dat2 &0x00ff) << 8) | ((dat2 &0xff00) >> 8);
				else if (mode==6) dat2 += extraxor;
				else
				{
					UINT16 extraxor2 = 0;
					if ((x & 0x003) == 0x000) extraxor2 |= 0x0049; // 'I'
					if ((x & 0x003) == 0x001) extraxor2 |= 0x0047; // 'G'
					if ((x & 0x003) == 0x002) extraxor2 |= 0x0053; // 'S'
					if ((x & 0x003) == 0x003) extraxor2 |= 0x0020; // ' '
					if ((x & 0x300) == 0x000) extraxor2 |= 0x4900; // 'I'
					if ((x & 0x300) == 0x100) extraxor2 |= 0x4700; // 'G'
					if ((x & 0x300) == 0x200) extraxor2 |= 0x5300; // 'S'
					if ((x & 0x300) == 0x300) extraxor2 |= 0x2000; // ' '

					dat2 = 0x4e75; // hack
				}

				sharedprotram[dst + x] = dat2;
			}
		}
		break;
	}
}

static void IGS028_handle()
{
	UINT16 cmd = sharedprotram[0x3026 / 2];

	switch (cmd)
	{
		case 0x12:
		{
			UINT16 mode = sharedprotram[0x303e / 2];  // ?
			UINT16 src  = sharedprotram[0x306a / 2] >> 1; // ?
			UINT16 dst  = sharedprotram[0x3084 / 2] & 0x1fff;
			UINT16 size = sharedprotram[0x30a2 / 2] & 0x1fff;

			IGS028_do_dma(src, dst, size, mode);
		}
		break;

		case 0x64: // incomplete?
		{
			UINT16 p1 = sharedprotram[0x3050 / 2];
			UINT16 p2 = sharedprotram[0x3082 / 2];
			UINT16 p3 = sharedprotram[0x3054 / 2];
			UINT16 p4 = sharedprotram[0x3088 / 2];

			if (p2  == 0x02)
				olds_write_reg(p1, olds_read_reg(p1) + 0x10000);

			switch (p4)
			{
				case 0xd:
					olds_write_reg(p1,olds_read_reg(p3));
				break;
				case 0x0:
					olds_write_reg(p3,(olds_read_reg(p2))^(olds_read_reg(p1)));
				break;
				case 0xe:
					olds_write_reg(p3,olds_read_reg(p3)+0x10000);
				break;
				case 0x2:
					olds_write_reg(p1,(olds_read_reg(p2))+(olds_read_reg(p3)));
				break;
				case 0x6:
					olds_write_reg(p3,(olds_read_reg(p2))&(olds_read_reg(p1)));
				break;
				case 0x1:
					olds_write_reg(p2,olds_read_reg(p1)+0x10000);
				break;
				case 0x7:
					olds_write_reg(p3,olds_read_reg(p1));
				break;
			}
		}
		break;
	}
}

static void killbld_protection_calculate_hold(INT32 y, INT32 z)
{
	UINT16 old = kb_prot_hold;

	kb_prot_hold = ((old << 1) | (old >> 15));

	kb_prot_hold ^= 0x2bad;
	kb_prot_hold ^= BIT(z, y);
	kb_prot_hold ^= BIT(old, 7) << 0;
	kb_prot_hold ^= BIT(~old, 13) << 4;
	kb_prot_hold ^= BIT(old, 3) << 11;

	kb_prot_hold ^= (kb_prot_hilo & ~0x0408) << 1;
}

static void killbld_protection_calculate_hilo()
{
	kb_prot_hilo_select++;

	if (kb_prot_hilo_select > 0xeb) {
		kb_prot_hilo_select = 0;
	}

	UINT8 source = source_data[kb_region][kb_prot_hilo_select];

	if (kb_prot_hilo_select & 1)
	{
		kb_prot_hilo = (kb_prot_hilo & 0x00ff) | (source << 8);
	}
	else
	{
		kb_prot_hilo = (kb_prot_hilo & 0xff00) | (source << 0);
	}
}

static void __fastcall killbld_igs025_prot_write(UINT32 address, UINT16 data)
{
	bprintf (0, _T("PRTW: %5.5x %4.4x\n"), address, data);

	if ((address & 0xf0000f) == 0xd00000) {
		kb_cmd = data;
		return;
	}

	switch (kb_cmd)
	{
		case 0x00:
			kb_reg = data;
		break;

		case 0x01: // drgw3
		{
			if (data == 0x0002) { // Execute command
				IGS022_handle_command();
			}
		}
		break;

		case 0x02: // killbld
		{
			if (data == 0x0001) { // Execute command
				IGS022_handle_command();
				kb_reg++;
			}
		}
		break;

		case 0x03:
			kb_swap = data;
		break;

		case 0x04:
	//      	kb_ptr = data; // Suspect. Not good for drgw3
		break;

		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			kb_ptr++;
			killbld_protection_calculate_hold(kb_cmd & 0x0f, data & 0xff);
		break;
	}
}

static void __fastcall olds_igs025_prot_write(UINT32 address, UINT16 data)
{
	bprintf (0, _T("PRTW: %5.5x %4.4x\n"), address, data);

	if (address == 0xdcb400) {
		kb_cmd = data;
		return;
	}

	switch (kb_cmd)
	{
		case 0x00:
			kb_reg = data;
		break;

		case 0x02:
			olds_bs = ((data & 0x03) << 6) | ((data & 0x04) << 3) | ((data & 0x08) << 1);
		break;

		case 0x03:
		{
			IGS028_handle();
			kb_cmd3 = ((data >> 4) + 1) & 0x3;
		}
		break;

		case 0x04:
			kb_ptr = data;
		break;

		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			kb_ptr++;
			killbld_protection_calculate_hold(kb_cmd & 0x0f, data & 0xff);
		break;
	}
}

static void __fastcall drgw2_igs025_prot_write(UINT32 address, UINT16 data)
{
	bprintf (0, _T("PRTW: %5.5x %4.4x\n"), address, data);

	if (address == 0xd80000) {
		kb_cmd = data;
		return;
	}

	switch (kb_cmd)
	{
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			kb_ptr++;
			killbld_protection_calculate_hold(kb_cmd & 0x0f, data & 0xff);
		break;
	}
}

static UINT16 __fastcall igs025_prot_read(UINT32 address)
{
	bprintf (0, _T("PRTR: %5.5x\n"), address);

	switch (kb_cmd)
	{
		case 0x00:
			return BITSWAP08((kb_swap + 1) & 0x7f, 0, 1, 2, 3, 4, 5, 6, 7); // drgw3

		case 0x01:
			return kb_reg & 0x7f;

		case 0x02:
			return olds_bs | 0x80;

		case 0x03:
			return kb_cmd3;

		case 0x05:
		{
			switch (kb_ptr)
			{
				case 1:
					return 0x3f00 | ((kb_game_id >> 0) & 0xff);

				case 2:
					return 0x3f00 | ((kb_game_id >> 8) & 0xff);

				case 3:
					return 0x3f00 | ((kb_game_id >> 16) & 0xff);

				case 4:
					return 0x3f00 | ((kb_game_id >> 24) & 0xff);

				default: // >= 5
					return 0x3f00 | BITSWAP08(kb_prot_hold, 5, 2, 9, 7, 10, 13, 12, 15);
			}
		}

		case 0x40:
			killbld_protection_calculate_hilo();
			return 0;
	}

	return 0;
}

static void common_reset()
{
	kb_region = PgmInput[7];

	kb_prot_hold = 0;
	kb_prot_hilo = 0;
	kb_prot_hilo_select = 0;
	kb_cmd = 0;
	kb_reg = 0;
	kb_ptr = 0;
	kb_swap = 0;
	olds_bs = 0;
	kb_cmd3 = 0;

	memset(kb_regs, 0, 0x100 * sizeof(UINT32));
}

static void drgw2_reset()
{
	common_reset();
}

static void killbld_reset()
{
	common_reset();

	kb_game_id = 0x89911400 | kb_region;

	IGS022_reset();
}

static void drgw3_reset()
{
	common_reset();

	kb_game_id = 0x00060000 | kb_region;

	IGS022_reset();
}

static INT32 CommonScan(INT32 nAction, INT32 *)
{
	struct BurnArea ba;

	if (nAction & ACB_MEMORY_RAM) {
		ba.Data		= PGMUSER0;
		ba.nLen		= 0x0004000;
		ba.nAddress	= 0x400000;
		ba.szName	= "ProtRAM";
		BurnAcb(&ba);

		ba.Data		= (UINT8*)kb_regs;
		ba.nLen		= 0x00100 * sizeof(INT32);
		ba.nAddress	= 0xfffffc00;
		ba.szName	= "Protection Registers";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(kb_prot_hold);
		SCAN_VAR(kb_prot_hilo);
		SCAN_VAR(kb_ptr);
		SCAN_VAR(kb_region);
		SCAN_VAR(kb_cmd);
		SCAN_VAR(kb_reg);
		SCAN_VAR(kb_swap);
		SCAN_VAR(kb_cmd3);
		SCAN_VAR(olds_bs);
		SCAN_VAR(kb_prot_hilo_select);
		SCAN_VAR(kb_game_id);
	}

	return 0;
}

static void olds_reset()
{
	common_reset();

	if (strcmp(BurnDrvGetTextA(DRV_NAME), "drgw2")    == 0) kb_region = 6;
	if (strcmp(BurnDrvGetTextA(DRV_NAME), "dw2v100x") == 0) kb_region = 6;
	if (strcmp(BurnDrvGetTextA(DRV_NAME), "drgw2c")   == 0) kb_region = 5;
	if (strcmp(BurnDrvGetTextA(DRV_NAME), "drgw2j")   == 0) kb_region = 1;

	kb_game_id = 0x00900000 | kb_region;

	sharedprotram[0x1000/2] = 0x4749; // 'IGS.28'
	sharedprotram[0x1002/2] = 0x2E53;
	sharedprotram[0x1004/2] = 0x3832;

	sharedprotram[0x3064/2] = 0xB315; // crc?
}

void install_protection_asic25_asic22_killbld()
{
	BurnByteswap(PGMProtROM, 0x10000);

	pPgmScanCallback = CommonScan;
	pPgmResetCallback = killbld_reset;

	sharedprotram = (UINT16*)PGMUSER0;

	SekOpen(0);
	SekMapMemory(PGMUSER0,		0x300000, 0x303fff, MAP_RAM);
	SekMapHandler(4,		0xd40000, 0xd40003, MAP_READ | MAP_WRITE);
	SekSetReadWordHandler(4, 	igs025_prot_read);
	SekSetWriteWordHandler(4, 	killbld_igs025_prot_write);
	SekClose();
}

void install_protection_asic25_asic22_drgw3()
{
	BurnByteswap(PGMProtROM, 0x10000);

	pPgmScanCallback = CommonScan;
	pPgmResetCallback = drgw3_reset;

	sharedprotram = (UINT16*)PGMUSER0;

	SekOpen(0);
	SekMapMemory(PGMUSER0,		0x300000, 0x303fff, MAP_RAM);
	SekMapHandler(4,		0xda5610, 0xda5613, MAP_READ | MAP_WRITE);
	SekSetReadWordHandler(4, 	igs025_prot_read);
	SekSetWriteWordHandler(4, 	killbld_igs025_prot_write);
	SekClose();
}

void install_protection_asic25_asic12_dw2()
{
	pPgmScanCallback = CommonScan;
	pPgmResetCallback = drgw2_reset;

	SekOpen(0);
	SekMapHandler(4,		0xd80000, 0xd80003, MAP_READ | MAP_WRITE);
	SekSetReadWordHandler(4,	igs025_prot_read);
	SekSetWriteWordHandler(4,	drgw2_igs025_prot_write);
	SekClose();
}

void install_protection_asic25_asic28_olds()
{
	pPgmScanCallback = CommonScan;
	pPgmResetCallback = olds_reset;

	sharedprotram = (UINT16*)PGMUSER0;

	SekOpen(0);
	SekMapMemory(PGMUSER0,		0x400000, 0x403fff, MAP_RAM);
	SekMapHandler(4,	  	0xdcb400, 0xdcb403, MAP_READ | MAP_WRITE);
	SekSetReadWordHandler(4,  	igs025_prot_read);
	SekSetWriteWordHandler(4, 	olds_igs025_prot_write);
	SekClose();
}
