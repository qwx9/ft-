/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2000-2009  Josh Coalson
 * Copyright (C) 2011-2018  Xiph.Org Foundation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "private/crc.h"

/* CRC-8, poly = x^8 + x^2 + x^1 + x^0, init = 0 */

FLAC__uint8 const FLAC__crc8_table[256] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
	0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
	0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
	0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
	0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
	0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
	0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
	0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
	0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
	0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
	0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
	0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
	0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
	0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
	0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
	0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

/* CRC-16, poly = x^16 + x^15 + x^2 + x^0, init = 0 */

FLAC__uint16 const FLAC__crc16_table[8][256] = {
  { 0x0000,  0x8005,  0x800f,  0x000a,  0x801b,  0x001e,  0x0014,  0x8011,
	0x8033,  0x0036,  0x003c,  0x8039,  0x0028,  0x802d,  0x8027,  0x0022,
	0x8063,  0x0066,  0x006c,  0x8069,  0x0078,  0x807d,  0x8077,  0x0072,
	0x0050,  0x8055,  0x805f,  0x005a,  0x804b,  0x004e,  0x0044,  0x8041,
	0x80c3,  0x00c6,  0x00cc,  0x80c9,  0x00d8,  0x80dd,  0x80d7,  0x00d2,
	0x00f0,  0x80f5,  0x80ff,  0x00fa,  0x80eb,  0x00ee,  0x00e4,  0x80e1,
	0x00a0,  0x80a5,  0x80af,  0x00aa,  0x80bb,  0x00be,  0x00b4,  0x80b1,
	0x8093,  0x0096,  0x009c,  0x8099,  0x0088,  0x808d,  0x8087,  0x0082,
	0x8183,  0x0186,  0x018c,  0x8189,  0x0198,  0x819d,  0x8197,  0x0192,
	0x01b0,  0x81b5,  0x81bf,  0x01ba,  0x81ab,  0x01ae,  0x01a4,  0x81a1,
	0x01e0,  0x81e5,  0x81ef,  0x01ea,  0x81fb,  0x01fe,  0x01f4,  0x81f1,
	0x81d3,  0x01d6,  0x01dc,  0x81d9,  0x01c8,  0x81cd,  0x81c7,  0x01c2,
	0x0140,  0x8145,  0x814f,  0x014a,  0x815b,  0x015e,  0x0154,  0x8151,
	0x8173,  0x0176,  0x017c,  0x8179,  0x0168,  0x816d,  0x8167,  0x0162,
	0x8123,  0x0126,  0x012c,  0x8129,  0x0138,  0x813d,  0x8137,  0x0132,
	0x0110,  0x8115,  0x811f,  0x011a,  0x810b,  0x010e,  0x0104,  0x8101,
	0x8303,  0x0306,  0x030c,  0x8309,  0x0318,  0x831d,  0x8317,  0x0312,
	0x0330,  0x8335,  0x833f,  0x033a,  0x832b,  0x032e,  0x0324,  0x8321,
	0x0360,  0x8365,  0x836f,  0x036a,  0x837b,  0x037e,  0x0374,  0x8371,
	0x8353,  0x0356,  0x035c,  0x8359,  0x0348,  0x834d,  0x8347,  0x0342,
	0x03c0,  0x83c5,  0x83cf,  0x03ca,  0x83db,  0x03de,  0x03d4,  0x83d1,
	0x83f3,  0x03f6,  0x03fc,  0x83f9,  0x03e8,  0x83ed,  0x83e7,  0x03e2,
	0x83a3,  0x03a6,  0x03ac,  0x83a9,  0x03b8,  0x83bd,  0x83b7,  0x03b2,
	0x0390,  0x8395,  0x839f,  0x039a,  0x838b,  0x038e,  0x0384,  0x8381,
	0x0280,  0x8285,  0x828f,  0x028a,  0x829b,  0x029e,  0x0294,  0x8291,
	0x82b3,  0x02b6,  0x02bc,  0x82b9,  0x02a8,  0x82ad,  0x82a7,  0x02a2,
	0x82e3,  0x02e6,  0x02ec,  0x82e9,  0x02f8,  0x82fd,  0x82f7,  0x02f2,
	0x02d0,  0x82d5,  0x82df,  0x02da,  0x82cb,  0x02ce,  0x02c4,  0x82c1,
	0x8243,  0x0246,  0x024c,  0x8249,  0x0258,  0x825d,  0x8257,  0x0252,
	0x0270,  0x8275,  0x827f,  0x027a,  0x826b,  0x026e,  0x0264,  0x8261,
	0x0220,  0x8225,  0x822f,  0x022a,  0x823b,  0x023e,  0x0234,  0x8231,
	0x8213,  0x0216,  0x021c,  0x8219,  0x0208,  0x820d,  0x8207,  0x0202 },

  { 0x0000,  0x8603,  0x8c03,  0x0a00,  0x9803,  0x1e00,  0x1400,  0x9203,
	0xb003,  0x3600,  0x3c00,  0xba03,  0x2800,  0xae03,  0xa403,  0x2200,
	0xe003,  0x6600,  0x6c00,  0xea03,  0x7800,  0xfe03,  0xf403,  0x7200,
	0x5000,  0xd603,  0xdc03,  0x5a00,  0xc803,  0x4e00,  0x4400,  0xc203,
	0x4003,  0xc600,  0xcc00,  0x4a03,  0xd800,  0x5e03,  0x5403,  0xd200,
	0xf000,  0x7603,  0x7c03,  0xfa00,  0x6803,  0xee00,  0xe400,  0x6203,
	0xa000,  0x2603,  0x2c03,  0xaa00,  0x3803,  0xbe00,  0xb400,  0x3203,
	0x1003,  0x9600,  0x9c00,  0x1a03,  0x8800,  0x0e03,  0x0403,  0x8200,
	0x8006,  0x0605,  0x0c05,  0x8a06,  0x1805,  0x9e06,  0x9406,  0x1205,
	0x3005,  0xb606,  0xbc06,  0x3a05,  0xa806,  0x2e05,  0x2405,  0xa206,
	0x6005,  0xe606,  0xec06,  0x6a05,  0xf806,  0x7e05,  0x7405,  0xf206,
	0xd006,  0x5605,  0x5c05,  0xda06,  0x4805,  0xce06,  0xc406,  0x4205,
	0xc005,  0x4606,  0x4c06,  0xca05,  0x5806,  0xde05,  0xd405,  0x5206,
	0x7006,  0xf605,  0xfc05,  0x7a06,  0xe805,  0x6e06,  0x6406,  0xe205,
	0x2006,  0xa605,  0xac05,  0x2a06,  0xb805,  0x3e06,  0x3406,  0xb205,
	0x9005,  0x1606,  0x1c06,  0x9a05,  0x0806,  0x8e05,  0x8405,  0x0206,
	0x8009,  0x060a,  0x0c0a,  0x8a09,  0x180a,  0x9e09,  0x9409,  0x120a,
	0x300a,  0xb609,  0xbc09,  0x3a0a,  0xa809,  0x2e0a,  0x240a,  0xa209,
	0x600a,  0xe609,  0xec09,  0x6a0a,  0xf809,  0x7e0a,  0x740a,  0xf209,
	0xd009,  0x560a,  0x5c0a,  0xda09,  0x480a,  0xce09,  0xc409,  0x420a,
	0xc00a,  0x4609,  0x4c09,  0xca0a,  0x5809,  0xde0a,  0xd40a,  0x5209,
	0x7009,  0xf60a,  0xfc0a,  0x7a09,  0xe80a,  0x6e09,  0x6409,  0xe20a,
	0x2009,  0xa60a,  0xac0a,  0x2a09,  0xb80a,  0x3e09,  0x3409,  0xb20a,
	0x900a,  0x1609,  0x1c09,  0x9a0a,  0x0809,  0x8e0a,  0x840a,  0x0209,
	0x000f,  0x860c,  0x8c0c,  0x0a0f,  0x980c,  0x1e0f,  0x140f,  0x920c,
	0xb00c,  0x360f,  0x3c0f,  0xba0c,  0x280f,  0xae0c,  0xa40c,  0x220f,
	0xe00c,  0x660f,  0x6c0f,  0xea0c,  0x780f,  0xfe0c,  0xf40c,  0x720f,
	0x500f,  0xd60c,  0xdc0c,  0x5a0f,  0xc80c,  0x4e0f,  0x440f,  0xc20c,
	0x400c,  0xc60f,  0xcc0f,  0x4a0c,  0xd80f,  0x5e0c,  0x540c,  0xd20f,
	0xf00f,  0x760c,  0x7c0c,  0xfa0f,  0x680c,  0xee0f,  0xe40f,  0x620c,
	0xa00f,  0x260c,  0x2c0c,  0xaa0f,  0x380c,  0xbe0f,  0xb40f,  0x320c,
	0x100c,  0x960f,  0x9c0f,  0x1a0c,  0x880f,  0x0e0c,  0x040c,  0x820f },

  { 0x0000,  0x8017,  0x802b,  0x003c,  0x8053,  0x0044,  0x0078,  0x806f,
	0x80a3,  0x00b4,  0x0088,  0x809f,  0x00f0,  0x80e7,  0x80db,  0x00cc,
	0x8143,  0x0154,  0x0168,  0x817f,  0x0110,  0x8107,  0x813b,  0x012c,
	0x01e0,  0x81f7,  0x81cb,  0x01dc,  0x81b3,  0x01a4,  0x0198,  0x818f,
	0x8283,  0x0294,  0x02a8,  0x82bf,  0x02d0,  0x82c7,  0x82fb,  0x02ec,
	0x0220,  0x8237,  0x820b,  0x021c,  0x8273,  0x0264,  0x0258,  0x824f,
	0x03c0,  0x83d7,  0x83eb,  0x03fc,  0x8393,  0x0384,  0x03b8,  0x83af,
	0x8363,  0x0374,  0x0348,  0x835f,  0x0330,  0x8327,  0x831b,  0x030c,
	0x8503,  0x0514,  0x0528,  0x853f,  0x0550,  0x8547,  0x857b,  0x056c,
	0x05a0,  0x85b7,  0x858b,  0x059c,  0x85f3,  0x05e4,  0x05d8,  0x85cf,
	0x0440,  0x8457,  0x846b,  0x047c,  0x8413,  0x0404,  0x0438,  0x842f,
	0x84e3,  0x04f4,  0x04c8,  0x84df,  0x04b0,  0x84a7,  0x849b,  0x048c,
	0x0780,  0x8797,  0x87ab,  0x07bc,  0x87d3,  0x07c4,  0x07f8,  0x87ef,
	0x8723,  0x0734,  0x0708,  0x871f,  0x0770,  0x8767,  0x875b,  0x074c,
	0x86c3,  0x06d4,  0x06e8,  0x86ff,  0x0690,  0x8687,  0x86bb,  0x06ac,
	0x0660,  0x8677,  0x864b,  0x065c,  0x8633,  0x0624,  0x0618,  0x860f,
	0x8a03,  0x0a14,  0x0a28,  0x8a3f,  0x0a50,  0x8a47,  0x8a7b,  0x0a6c,
	0x0aa0,  0x8ab7,  0x8a8b,  0x0a9c,  0x8af3,  0x0ae4,  0x0ad8,  0x8acf,
	0x0b40,  0x8b57,  0x8b6b,  0x0b7c,  0x8b13,  0x0b04,  0x0b38,  0x8b2f,
	0x8be3,  0x0bf4,  0x0bc8,  0x8bdf,  0x0bb0,  0x8ba7,  0x8b9b,  0x0b8c,
	0x0880,  0x8897,  0x88ab,  0x08bc,  0x88d3,  0x08c4,  0x08f8,  0x88ef,
	0x8823,  0x0834,  0x0808,  0x881f,  0x0870,  0x8867,  0x885b,  0x084c,
	0x89c3,  0x09d4,  0x09e8,  0x89ff,  0x0990,  0x8987,  0x89bb,  0x09ac,
	0x0960,  0x8977,  0x894b,  0x095c,  0x8933,  0x0924,  0x0918,  0x890f,
	0x0f00,  0x8f17,  0x8f2b,  0x0f3c,  0x8f53,  0x0f44,  0x0f78,  0x8f6f,
	0x8fa3,  0x0fb4,  0x0f88,  0x8f9f,  0x0ff0,  0x8fe7,  0x8fdb,  0x0fcc,
	0x8e43,  0x0e54,  0x0e68,  0x8e7f,  0x0e10,  0x8e07,  0x8e3b,  0x0e2c,
	0x0ee0,  0x8ef7,  0x8ecb,  0x0edc,  0x8eb3,  0x0ea4,  0x0e98,  0x8e8f,
	0x8d83,  0x0d94,  0x0da8,  0x8dbf,  0x0dd0,  0x8dc7,  0x8dfb,  0x0dec,
	0x0d20,  0x8d37,  0x8d0b,  0x0d1c,  0x8d73,  0x0d64,  0x0d58,  0x8d4f,
	0x0cc0,  0x8cd7,  0x8ceb,  0x0cfc,  0x8c93,  0x0c84,  0x0cb8,  0x8caf,
	0x8c63,  0x0c74,  0x0c48,  0x8c5f,  0x0c30,  0x8c27,  0x8c1b,  0x0c0c },

  { 0x0000,  0x9403,  0xa803,  0x3c00,  0xd003,  0x4400,  0x7800,  0xec03,
	0x2003,  0xb400,  0x8800,  0x1c03,  0xf000,  0x6403,  0x5803,  0xcc00,
	0x4006,  0xd405,  0xe805,  0x7c06,  0x9005,  0x0406,  0x3806,  0xac05,
	0x6005,  0xf406,  0xc806,  0x5c05,  0xb006,  0x2405,  0x1805,  0x8c06,
	0x800c,  0x140f,  0x280f,  0xbc0c,  0x500f,  0xc40c,  0xf80c,  0x6c0f,
	0xa00f,  0x340c,  0x080c,  0x9c0f,  0x700c,  0xe40f,  0xd80f,  0x4c0c,
	0xc00a,  0x5409,  0x6809,  0xfc0a,  0x1009,  0x840a,  0xb80a,  0x2c09,
	0xe009,  0x740a,  0x480a,  0xdc09,  0x300a,  0xa409,  0x9809,  0x0c0a,
	0x801d,  0x141e,  0x281e,  0xbc1d,  0x501e,  0xc41d,  0xf81d,  0x6c1e,
	0xa01e,  0x341d,  0x081d,  0x9c1e,  0x701d,  0xe41e,  0xd81e,  0x4c1d,
	0xc01b,  0x5418,  0x6818,  0xfc1b,  0x1018,  0x841b,  0xb81b,  0x2c18,
	0xe018,  0x741b,  0x481b,  0xdc18,  0x301b,  0xa418,  0x9818,  0x0c1b,
	0x0011,  0x9412,  0xa812,  0x3c11,  0xd012,  0x4411,  0x7811,  0xec12,
	0x2012,  0xb411,  0x8811,  0x1c12,  0xf011,  0x6412,  0x5812,  0xcc11,
	0x4017,  0xd414,  0xe814,  0x7c17,  0x9014,  0x0417,  0x3817,  0xac14,
	0x6014,  0xf417,  0xc817,  0x5c14,  0xb017,  0x2414,  0x1814,  0x8c17,
	0x803f,  0x143c,  0x283c,  0xbc3f,  0x503c,  0xc43f,  0xf83f,  0x6c3c,
	0xa03c,  0x343f,  0x083f,  0x9c3c,  0x703f,  0xe43c,  0xd83c,  0x4c3f,
	0xc039,  0x543a,  0x683a,  0xfc39,  0x103a,  0x8439,  0xb839,  0x2c3a,
	0xe03a,  0x7439,  0x4839,  0xdc3a,  0x3039,  0xa43a,  0x983a,  0x0c39,
	0x0033,  0x9430,  0xa830,  0x3c33,  0xd030,  0x4433,  0x7833,  0xec30,
	0x2030,  0xb433,  0x8833,  0x1c30,  0xf033,  0x6430,  0x5830,  0xcc33,
	0x4035,  0xd436,  0xe836,  0x7c35,  0x9036,  0x0435,  0x3835,  0xac36,
	0x6036,  0xf435,  0xc835,  0x5c36,  0xb035,  0x2436,  0x1836,  0x8c35,
	0x0022,  0x9421,  0xa821,  0x3c22,  0xd021,  0x4422,  0x7822,  0xec21,
	0x2021,  0xb422,  0x8822,  0x1c21,  0xf022,  0x6421,  0x5821,  0xcc22,
	0x4024,  0xd427,  0xe827,  0x7c24,  0x9027,  0x0424,  0x3824,  0xac27,
	0x6027,  0xf424,  0xc824,  0x5c27,  0xb024,  0x2427,  0x1827,  0x8c24,
	0x802e,  0x142d,  0x282d,  0xbc2e,  0x502d,  0xc42e,  0xf82e,  0x6c2d,
	0xa02d,  0x342e,  0x082e,  0x9c2d,  0x702e,  0xe42d,  0xd82d,  0x4c2e,
	0xc028,  0x542b,  0x682b,  0xfc28,  0x102b,  0x8428,  0xb828,  0x2c2b,
	0xe02b,  0x7428,  0x4828,  0xdc2b,  0x3028,  0xa42b,  0x982b,  0x0c28 },

  { 0x0000,  0x807b,  0x80f3,  0x0088,  0x81e3,  0x0198,  0x0110,  0x816b,
	0x83c3,  0x03b8,  0x0330,  0x834b,  0x0220,  0x825b,  0x82d3,  0x02a8,
	0x8783,  0x07f8,  0x0770,  0x870b,  0x0660,  0x861b,  0x8693,  0x06e8,
	0x0440,  0x843b,  0x84b3,  0x04c8,  0x85a3,  0x05d8,  0x0550,  0x852b,
	0x8f03,  0x0f78,  0x0ff0,  0x8f8b,  0x0ee0,  0x8e9b,  0x8e13,  0x0e68,
	0x0cc0,  0x8cbb,  0x8c33,  0x0c48,  0x8d23,  0x0d58,  0x0dd0,  0x8dab,
	0x0880,  0x88fb,  0x8873,  0x0808,  0x8963,  0x0918,  0x0990,  0x89eb,
	0x8b43,  0x0b38,  0x0bb0,  0x8bcb,  0x0aa0,  0x8adb,  0x8a53,  0x0a28,
	0x9e03,  0x1e78,  0x1ef0,  0x9e8b,  0x1fe0,  0x9f9b,  0x9f13,  0x1f68,
	0x1dc0,  0x9dbb,  0x9d33,  0x1d48,  0x9c23,  0x1c58,  0x1cd0,  0x9cab,
	0x1980,  0x99fb,  0x9973,  0x1908,  0x9863,  0x1818,  0x1890,  0x98eb,
	0x9a43,  0x1a38,  0x1ab0,  0x9acb,  0x1ba0,  0x9bdb,  0x9b53,  0x1b28,
	0x1100,  0x917b,  0x91f3,  0x1188,  0x90e3,  0x1098,  0x1010,  0x906b,
	0x92c3,  0x12b8,  0x1230,  0x924b,  0x1320,  0x935b,  0x93d3,  0x13a8,
	0x9683,  0x16f8,  0x1670,  0x960b,  0x1760,  0x971b,  0x9793,  0x17e8,
	0x1540,  0x953b,  0x95b3,  0x15c8,  0x94a3,  0x14d8,  0x1450,  0x942b,
	0xbc03,  0x3c78,  0x3cf0,  0xbc8b,  0x3de0,  0xbd9b,  0xbd13,  0x3d68,
	0x3fc0,  0xbfbb,  0xbf33,  0x3f48,  0xbe23,  0x3e58,  0x3ed0,  0xbeab,
	0x3b80,  0xbbfb,  0xbb73,  0x3b08,  0xba63,  0x3a18,  0x3a90,  0xbaeb,
	0xb843,  0x3838,  0x38b0,  0xb8cb,  0x39a0,  0xb9db,  0xb953,  0x3928,
	0x3300,  0xb37b,  0xb3f3,  0x3388,  0xb2e3,  0x3298,  0x3210,  0xb26b,
	0xb0c3,  0x30b8,  0x3030,  0xb04b,  0x3120,  0xb15b,  0xb1d3,  0x31a8,
	0xb483,  0x34f8,  0x3470,  0xb40b,  0x3560,  0xb51b,  0xb593,  0x35e8,
	0x3740,  0xb73b,  0xb7b3,  0x37c8,  0xb6a3,  0x36d8,  0x3650,  0xb62b,
	0x2200,  0xa27b,  0xa2f3,  0x2288,  0xa3e3,  0x2398,  0x2310,  0xa36b,
	0xa1c3,  0x21b8,  0x2130,  0xa14b,  0x2020,  0xa05b,  0xa0d3,  0x20a8,
	0xa583,  0x25f8,  0x2570,  0xa50b,  0x2460,  0xa41b,  0xa493,  0x24e8,
	0x2640,  0xa63b,  0xa6b3,  0x26c8,  0xa7a3,  0x27d8,  0x2750,  0xa72b,
	0xad03,  0x2d78,  0x2df0,  0xad8b,  0x2ce0,  0xac9b,  0xac13,  0x2c68,
	0x2ec0,  0xaebb,  0xae33,  0x2e48,  0xaf23,  0x2f58,  0x2fd0,  0xafab,
	0x2a80,  0xaafb,  0xaa73,  0x2a08,  0xab63,  0x2b18,  0x2b90,  0xabeb,
	0xa943,  0x2938,  0x29b0,  0xa9cb,  0x28a0,  0xa8db,  0xa853,  0x2828 },

  { 0x0000,  0xf803,  0x7003,  0x8800,  0xe006,  0x1805,  0x9005,  0x6806,
	0x4009,  0xb80a,  0x300a,  0xc809,  0xa00f,  0x580c,  0xd00c,  0x280f,
	0x8012,  0x7811,  0xf011,  0x0812,  0x6014,  0x9817,  0x1017,  0xe814,
	0xc01b,  0x3818,  0xb018,  0x481b,  0x201d,  0xd81e,  0x501e,  0xa81d,
	0x8021,  0x7822,  0xf022,  0x0821,  0x6027,  0x9824,  0x1024,  0xe827,
	0xc028,  0x382b,  0xb02b,  0x4828,  0x202e,  0xd82d,  0x502d,  0xa82e,
	0x0033,  0xf830,  0x7030,  0x8833,  0xe035,  0x1836,  0x9036,  0x6835,
	0x403a,  0xb839,  0x3039,  0xc83a,  0xa03c,  0x583f,  0xd03f,  0x283c,
	0x8047,  0x7844,  0xf044,  0x0847,  0x6041,  0x9842,  0x1042,  0xe841,
	0xc04e,  0x384d,  0xb04d,  0x484e,  0x2048,  0xd84b,  0x504b,  0xa848,
	0x0055,  0xf856,  0x7056,  0x8855,  0xe053,  0x1850,  0x9050,  0x6853,
	0x405c,  0xb85f,  0x305f,  0xc85c,  0xa05a,  0x5859,  0xd059,  0x285a,
	0x0066,  0xf865,  0x7065,  0x8866,  0xe060,  0x1863,  0x9063,  0x6860,
	0x406f,  0xb86c,  0x306c,  0xc86f,  0xa069,  0x586a,  0xd06a,  0x2869,
	0x8074,  0x7877,  0xf077,  0x0874,  0x6072,  0x9871,  0x1071,  0xe872,
	0xc07d,  0x387e,  0xb07e,  0x487d,  0x207b,  0xd878,  0x5078,  0xa87b,
	0x808b,  0x7888,  0xf088,  0x088b,  0x608d,  0x988e,  0x108e,  0xe88d,
	0xc082,  0x3881,  0xb081,  0x4882,  0x2084,  0xd887,  0x5087,  0xa884,
	0x0099,  0xf89a,  0x709a,  0x8899,  0xe09f,  0x189c,  0x909c,  0x689f,
	0x4090,  0xb893,  0x3093,  0xc890,  0xa096,  0x5895,  0xd095,  0x2896,
	0x00aa,  0xf8a9,  0x70a9,  0x88aa,  0xe0ac,  0x18af,  0x90af,  0x68ac,
	0x40a3,  0xb8a0,  0x30a0,  0xc8a3,  0xa0a5,  0x58a6,  0xd0a6,  0x28a5,
	0x80b8,  0x78bb,  0xf0bb,  0x08b8,  0x60be,  0x98bd,  0x10bd,  0xe8be,
	0xc0b1,  0x38b2,  0xb0b2,  0x48b1,  0x20b7,  0xd8b4,  0x50b4,  0xa8b7,
	0x00cc,  0xf8cf,  0x70cf,  0x88cc,  0xe0ca,  0x18c9,  0x90c9,  0x68ca,
	0x40c5,  0xb8c6,  0x30c6,  0xc8c5,  0xa0c3,  0x58c0,  0xd0c0,  0x28c3,
	0x80de,  0x78dd,  0xf0dd,  0x08de,  0x60d8,  0x98db,  0x10db,  0xe8d8,
	0xc0d7,  0x38d4,  0xb0d4,  0x48d7,  0x20d1,  0xd8d2,  0x50d2,  0xa8d1,
	0x80ed,  0x78ee,  0xf0ee,  0x08ed,  0x60eb,  0x98e8,  0x10e8,  0xe8eb,
	0xc0e4,  0x38e7,  0xb0e7,  0x48e4,  0x20e2,  0xd8e1,  0x50e1,  0xa8e2,
	0x00ff,  0xf8fc,  0x70fc,  0x88ff,  0xe0f9,  0x18fa,  0x90fa,  0x68f9,
	0x40f6,  0xb8f5,  0x30f5,  0xc8f6,  0xa0f0,  0x58f3,  0xd0f3,  0x28f0 },

  { 0x0000,  0x8113,  0x8223,  0x0330,  0x8443,  0x0550,  0x0660,  0x8773,
	0x8883,  0x0990,  0x0aa0,  0x8bb3,  0x0cc0,  0x8dd3,  0x8ee3,  0x0ff0,
	0x9103,  0x1010,  0x1320,  0x9233,  0x1540,  0x9453,  0x9763,  0x1670,
	0x1980,  0x9893,  0x9ba3,  0x1ab0,  0x9dc3,  0x1cd0,  0x1fe0,  0x9ef3,
	0xa203,  0x2310,  0x2020,  0xa133,  0x2640,  0xa753,  0xa463,  0x2570,
	0x2a80,  0xab93,  0xa8a3,  0x29b0,  0xaec3,  0x2fd0,  0x2ce0,  0xadf3,
	0x3300,  0xb213,  0xb123,  0x3030,  0xb743,  0x3650,  0x3560,  0xb473,
	0xbb83,  0x3a90,  0x39a0,  0xb8b3,  0x3fc0,  0xbed3,  0xbde3,  0x3cf0,
	0xc403,  0x4510,  0x4620,  0xc733,  0x4040,  0xc153,  0xc263,  0x4370,
	0x4c80,  0xcd93,  0xcea3,  0x4fb0,  0xc8c3,  0x49d0,  0x4ae0,  0xcbf3,
	0x5500,  0xd413,  0xd723,  0x5630,  0xd143,  0x5050,  0x5360,  0xd273,
	0xdd83,  0x5c90,  0x5fa0,  0xdeb3,  0x59c0,  0xd8d3,  0xdbe3,  0x5af0,
	0x6600,  0xe713,  0xe423,  0x6530,  0xe243,  0x6350,  0x6060,  0xe173,
	0xee83,  0x6f90,  0x6ca0,  0xedb3,  0x6ac0,  0xebd3,  0xe8e3,  0x69f0,
	0xf703,  0x7610,  0x7520,  0xf433,  0x7340,  0xf253,  0xf163,  0x7070,
	0x7f80,  0xfe93,  0xfda3,  0x7cb0,  0xfbc3,  0x7ad0,  0x79e0,  0xf8f3,
	0x0803,  0x8910,  0x8a20,  0x0b33,  0x8c40,  0x0d53,  0x0e63,  0x8f70,
	0x8080,  0x0193,  0x02a3,  0x83b0,  0x04c3,  0x85d0,  0x86e0,  0x07f3,
	0x9900,  0x1813,  0x1b23,  0x9a30,  0x1d43,  0x9c50,  0x9f60,  0x1e73,
	0x1183,  0x9090,  0x93a0,  0x12b3,  0x95c0,  0x14d3,  0x17e3,  0x96f0,
	0xaa00,  0x2b13,  0x2823,  0xa930,  0x2e43,  0xaf50,  0xac60,  0x2d73,
	0x2283,  0xa390,  0xa0a0,  0x21b3,  0xa6c0,  0x27d3,  0x24e3,  0xa5f0,
	0x3b03,  0xba10,  0xb920,  0x3833,  0xbf40,  0x3e53,  0x3d63,  0xbc70,
	0xb380,  0x3293,  0x31a3,  0xb0b0,  0x37c3,  0xb6d0,  0xb5e0,  0x34f3,
	0xcc00,  0x4d13,  0x4e23,  0xcf30,  0x4843,  0xc950,  0xca60,  0x4b73,
	0x4483,  0xc590,  0xc6a0,  0x47b3,  0xc0c0,  0x41d3,  0x42e3,  0xc3f0,
	0x5d03,  0xdc10,  0xdf20,  0x5e33,  0xd940,  0x5853,  0x5b63,  0xda70,
	0xd580,  0x5493,  0x57a3,  0xd6b0,  0x51c3,  0xd0d0,  0xd3e0,  0x52f3,
	0x6e03,  0xef10,  0xec20,  0x6d33,  0xea40,  0x6b53,  0x6863,  0xe970,
	0xe680,  0x6793,  0x64a3,  0xe5b0,  0x62c3,  0xe3d0,  0xe0e0,  0x61f3,
	0xff00,  0x7e13,  0x7d23,  0xfc30,  0x7b43,  0xfa50,  0xf960,  0x7873,
	0x7783,  0xf690,  0xf5a0,  0x74b3,  0xf3c0,  0x72d3,  0x71e3,  0xf0f0 },

  { 0x0000,  0x1006,  0x200c,  0x300a,  0x4018,  0x501e,  0x6014,  0x7012,
	0x8030,  0x9036,  0xa03c,  0xb03a,  0xc028,  0xd02e,  0xe024,  0xf022,
	0x8065,  0x9063,  0xa069,  0xb06f,  0xc07d,  0xd07b,  0xe071,  0xf077,
	0x0055,  0x1053,  0x2059,  0x305f,  0x404d,  0x504b,  0x6041,  0x7047,
	0x80cf,  0x90c9,  0xa0c3,  0xb0c5,  0xc0d7,  0xd0d1,  0xe0db,  0xf0dd,
	0x00ff,  0x10f9,  0x20f3,  0x30f5,  0x40e7,  0x50e1,  0x60eb,  0x70ed,
	0x00aa,  0x10ac,  0x20a6,  0x30a0,  0x40b2,  0x50b4,  0x60be,  0x70b8,
	0x809a,  0x909c,  0xa096,  0xb090,  0xc082,  0xd084,  0xe08e,  0xf088,
	0x819b,  0x919d,  0xa197,  0xb191,  0xc183,  0xd185,  0xe18f,  0xf189,
	0x01ab,  0x11ad,  0x21a7,  0x31a1,  0x41b3,  0x51b5,  0x61bf,  0x71b9,
	0x01fe,  0x11f8,  0x21f2,  0x31f4,  0x41e6,  0x51e0,  0x61ea,  0x71ec,
	0x81ce,  0x91c8,  0xa1c2,  0xb1c4,  0xc1d6,  0xd1d0,  0xe1da,  0xf1dc,
	0x0154,  0x1152,  0x2158,  0x315e,  0x414c,  0x514a,  0x6140,  0x7146,
	0x8164,  0x9162,  0xa168,  0xb16e,  0xc17c,  0xd17a,  0xe170,  0xf176,
	0x8131,  0x9137,  0xa13d,  0xb13b,  0xc129,  0xd12f,  0xe125,  0xf123,
	0x0101,  0x1107,  0x210d,  0x310b,  0x4119,  0x511f,  0x6115,  0x7113,
	0x8333,  0x9335,  0xa33f,  0xb339,  0xc32b,  0xd32d,  0xe327,  0xf321,
	0x0303,  0x1305,  0x230f,  0x3309,  0x431b,  0x531d,  0x6317,  0x7311,
	0x0356,  0x1350,  0x235a,  0x335c,  0x434e,  0x5348,  0x6342,  0x7344,
	0x8366,  0x9360,  0xa36a,  0xb36c,  0xc37e,  0xd378,  0xe372,  0xf374,
	0x03fc,  0x13fa,  0x23f0,  0x33f6,  0x43e4,  0x53e2,  0x63e8,  0x73ee,
	0x83cc,  0x93ca,  0xa3c0,  0xb3c6,  0xc3d4,  0xd3d2,  0xe3d8,  0xf3de,
	0x8399,  0x939f,  0xa395,  0xb393,  0xc381,  0xd387,  0xe38d,  0xf38b,
	0x03a9,  0x13af,  0x23a5,  0x33a3,  0x43b1,  0x53b7,  0x63bd,  0x73bb,
	0x02a8,  0x12ae,  0x22a4,  0x32a2,  0x42b0,  0x52b6,  0x62bc,  0x72ba,
	0x8298,  0x929e,  0xa294,  0xb292,  0xc280,  0xd286,  0xe28c,  0xf28a,
	0x82cd,  0x92cb,  0xa2c1,  0xb2c7,  0xc2d5,  0xd2d3,  0xe2d9,  0xf2df,
	0x02fd,  0x12fb,  0x22f1,  0x32f7,  0x42e5,  0x52e3,  0x62e9,  0x72ef,
	0x8267,  0x9261,  0xa26b,  0xb26d,  0xc27f,  0xd279,  0xe273,  0xf275,
	0x0257,  0x1251,  0x225b,  0x325d,  0x424f,  0x5249,  0x6243,  0x7245,
	0x0202,  0x1204,  0x220e,  0x3208,  0x421a,  0x521c,  0x6216,  0x7210,
	0x8232,  0x9234,  0xa23e,  0xb238,  0xc22a,  0xd22c,  0xe226,  0xf220 }
};

FLAC__uint8 FLAC__crc8(const FLAC__byte *data, uint32_t len)
{
	FLAC__uint8 crc = 0;

	while(len--)
		crc = FLAC__crc8_table[crc ^ *data++];

	return crc;
}

FLAC__uint16 FLAC__crc16(const FLAC__byte *data, uint32_t len)
{
	FLAC__uint16 crc = 0;

	while(len >= 8){
		crc ^= data[0] << 8 | data[1];

		crc = FLAC__crc16_table[7][crc >> 8] ^ FLAC__crc16_table[6][crc & 0xFF] ^
		      FLAC__crc16_table[5][data[2] ] ^ FLAC__crc16_table[4][data[3]   ] ^
		      FLAC__crc16_table[3][data[4] ] ^ FLAC__crc16_table[2][data[5]   ] ^
		      FLAC__crc16_table[1][data[6] ] ^ FLAC__crc16_table[0][data[7]   ];

		data += 8;
		len -= 8;
	}

	while(len--)
		crc = (crc<<8) ^ FLAC__crc16_table[0][(crc>>8) ^ *data++];

	return crc;
}

FLAC__uint16 FLAC__crc16_update_words32(const FLAC__uint32 *words, uint32_t len, FLAC__uint16 crc)
{
	while (len >= 2) {
		crc ^= words[0] >> 16;

		crc = FLAC__crc16_table[7][crc >> 8               ] ^ FLAC__crc16_table[6][crc & 0xFF             ] ^
		      FLAC__crc16_table[5][(words[0] >>  8) & 0xFF] ^ FLAC__crc16_table[4][ words[0]        & 0xFF] ^
		      FLAC__crc16_table[3][ words[1] >> 24        ] ^ FLAC__crc16_table[2][(words[1] >> 16) & 0xFF] ^
		      FLAC__crc16_table[1][(words[1] >>  8) & 0xFF] ^ FLAC__crc16_table[0][ words[1]        & 0xFF];

		words += 2;
		len -= 2;
	}

	if (len) {
		crc ^= words[0] >> 16;

		crc = FLAC__crc16_table[3][crc >> 8               ] ^ FLAC__crc16_table[2][crc & 0xFF             ] ^
		      FLAC__crc16_table[1][(words[0] >>  8) & 0xFF] ^ FLAC__crc16_table[0][words[0]         & 0xFF];
	}

	return crc;
}

FLAC__uint16 FLAC__crc16_update_words64(const FLAC__uint64 *words, uint32_t len, FLAC__uint16 crc)
{
	while (len--) {
		crc ^= words[0] >> 48;

		crc = FLAC__crc16_table[7][crc >> 8               ] ^ FLAC__crc16_table[6][crc & 0xFF             ] ^
		      FLAC__crc16_table[5][(words[0] >> 40) & 0xFF] ^ FLAC__crc16_table[4][(words[0] >> 32) & 0xFF] ^
		      FLAC__crc16_table[3][(words[0] >> 24) & 0xFF] ^ FLAC__crc16_table[2][(words[0] >> 16) & 0xFF] ^
		      FLAC__crc16_table[1][(words[0] >>  8) & 0xFF] ^ FLAC__crc16_table[0][ words[0]        & 0xFF];

		words++;
	}

	return crc;
}
