
#include "consolamono.hpp"
#include "graphics/text/fontdata.hpp"

FontData& Fonts::setFontConsolaMono(FontData& fontdata) {
	fontdata.textureID = "FONT_CONSOLA_MONO";
    fontdata.totalWidth = 1024;
    fontdata.totalHeight = 1024;
    fontdata.baseLine = 108;
    fontdata.lineHeight = 130;


	fontdata[32] = Glyph(0, 980, 87  , 16  , 15  , -7  , 122 , 55 );
	fontdata[33] = Glyph(0, 978, 105 , 27  , 85  , 14  , 31  , 55 );
	fontdata[34] = Glyph(0, 620, 797 , 37  , 37  , 9   , 31  , 55 );
	fontdata[35] = Glyph(0, 54 , 749 , 53  , 68  , 1   , 39  , 55 );
	fontdata[36] = Glyph(0, 136, 0   , 57  , 108 , -1  , 19  , 55 );
	fontdata[37] = Glyph(0, 54 , 492 , 68  , 85  , -6  , 30  , 55 );
	fontdata[38] = Glyph(0, 490, 303 , 64  , 86  , -4  , 30  , 55 );
	fontdata[39] = Glyph(0, 724, 788 , 24  , 37  , 16  , 31  , 55 );
	fontdata[40] = Glyph(0, 380, 0   , 37  , 108 , 9   , 19  , 55 );
	fontdata[41] = Glyph(0, 418, 0   , 37  , 108 , 9   , 19  , 55 );
	fontdata[42] = Glyph(0, 214, 816 , 52  , 53  , 2   , 31  , 55 );
	fontdata[43] = Glyph(0, 306, 816 , 53  , 52  , 1   , 47  , 55 );
	fontdata[44] = Glyph(0, 998, 711 , 25  , 38  , 15  , 91  , 55 );
	fontdata[45] = Glyph(0, 108, 872 , 53  , 22  , 1   , 62  , 55 );
	fontdata[46] = Glyph(0, 900, 769 , 27  , 27  , 14  , 89  , 55 );
	fontdata[47] = Glyph(0, 0  , 0   , 67  , 108 , -6  , 19  , 55 );
	fontdata[48] = Glyph(0, 801, 298 , 60  , 86  , -2  , 30  , 55 );
	fontdata[49] = Glyph(0, 559, 649 , 53  , 84  , 4   , 31  , 55 );
	fontdata[50] = Glyph(0, 430, 479 , 57  , 85  , -1  , 30  , 55 );
	fontdata[51] = Glyph(0, 354, 392 , 57  , 86  , -1  , 30  , 55 );
	fontdata[52] = Glyph(0, 173, 663 , 56  , 84  , -3  , 31  , 55 );
	fontdata[53] = Glyph(0, 488, 478 , 57  , 85  , -1  , 31  , 55 );
	fontdata[54] = Glyph(0, 60 , 405 , 59  , 86  , -2  , 30  , 55 );
	fontdata[55] = Glyph(0, 58 , 664 , 57  , 84  , -1  , 31  , 55 );
	fontdata[56] = Glyph(0, 180, 405 , 57  , 86  , -1  , 30  , 55 );
	fontdata[57] = Glyph(0, 120, 405 , 59  , 86  , -2  , 30  , 55 );
	fontdata[58] = Glyph(0, 984, 297 , 27  , 57  , 14  , 59  , 55 );
	fontdata[59] = Glyph(0, 993, 191 , 27  , 70  , 14  , 59  , 55 );
	fontdata[60] = Glyph(0, 674, 729 , 52  , 58  , 2   , 44  , 55 );
	fontdata[61] = Glyph(0, 566, 801 , 53  , 37  , 1   , 54  , 55 );
	fontdata[62] = Glyph(0, 727, 713 , 52  , 58  , 2   , 44  , 55 );
	fontdata[63] = Glyph(0, 296, 405 , 57  , 86  , -1  , 30  , 55 );
	fontdata[64] = Glyph(0, 923, 298 , 60  , 86  , -2  , 48  , 55 );
	fontdata[65] = Glyph(0, 67 , 578 , 66  , 84  , -5  , 31  , 55 );
	fontdata[66] = Glyph(0, 0  , 664 , 57  , 84  , -1  , 31  , 55 );
	fontdata[67] = Glyph(0, 412, 392 , 57  , 86  , -2  , 30  , 55 );
	fontdata[68] = Glyph(0, 575, 563 , 59  , 84  , -1  , 31  , 55 );
	fontdata[69] = Glyph(0, 505, 650 , 53  , 84  , 1   , 31  , 55 );
	fontdata[70] = Glyph(0, 667, 644 , 50  , 84  , 3   , 31  , 55 );
	fontdata[71] = Glyph(0, 679, 299 , 60  , 86  , -2  , 30  , 55 );
	fontdata[72] = Glyph(0, 751, 559 , 57  , 84  , -1  , 31  , 55 );
	fontdata[73] = Glyph(0, 451, 650 , 53  , 84  , 1   , 31  , 55 );
	fontdata[74] = Glyph(0, 604, 473 , 57  , 85  , -3  , 31  , 55 );
	fontdata[75] = Glyph(0, 693, 559 , 57  , 84  , -1  , 31  , 55 );
	fontdata[76] = Glyph(0, 397, 650 , 53  , 84  , 1   , 31  , 55 );
	fontdata[77] = Glyph(0, 230, 663 , 56  , 84  , -1  , 31  , 55 );
	fontdata[78] = Glyph(0, 116, 663 , 56  , 84  , -1  , 31  , 55 );
	fontdata[79] = Glyph(0, 862, 298 , 60  , 86  , -2  , 30  , 55 );
	fontdata[80] = Glyph(0, 867, 558 , 57  , 84  , -1  , 31  , 55 );
	fontdata[81] = Glyph(0, 299, 214 , 61  , 89  , -2  , 30  , 55 );
	fontdata[82] = Glyph(0, 515, 564 , 59  , 84  , -1  , 31  , 55 );
	fontdata[83] = Glyph(0, 238, 405 , 57  , 86  , -1  , 30  , 55 );
	fontdata[84] = Glyph(0, 925, 557 , 57  , 84  , -1  , 31  , 55 );
	fontdata[85] = Glyph(0, 546, 477 , 57  , 85  , -1  , 31  , 55 );
	fontdata[86] = Glyph(0, 134, 578 , 66  , 84  , -5  , 31  , 55 );
	fontdata[87] = Glyph(0, 880, 472 , 69  , 84  , -7  , 31  , 55 );
	fontdata[88] = Glyph(0, 391, 565 , 61  , 84  , -3  , 31  , 55 );
	fontdata[89] = Glyph(0, 329, 578 , 61  , 84  , -3  , 31  , 55 );
	fontdata[90] = Glyph(0, 809, 558 , 57  , 84  , -1  , 31  , 55 );
	fontdata[91] = Glyph(0, 302, 0   , 38  , 108 , 9   , 19  , 55 );
	fontdata[92] = Glyph(0, 68 , 0   , 67  , 108 , -6  , 19  , 55 );
	fontdata[93] = Glyph(0, 341, 0   , 38  , 108 , 9   , 19  , 55 );
	fontdata[94] = Glyph(0, 749, 772 , 53  , 35  , 1   , 31  , 55 );
	fontdata[95] = Glyph(0, 928, 769 , 53  , 22  , 1   , 102 , 55 );
	fontdata[96] = Glyph(0, 658, 797 , 32  , 37  , 12  , 30  , 55 );
	fontdata[97] = Glyph(0, 0  , 749 , 53  , 68  , 1   , 48  , 55 );
	fontdata[98] = Glyph(0, 662, 473 , 54  , 85  , 1   , 31  , 55 );
	fontdata[99] = Glyph(0, 949, 642 , 53  , 68  , 0   , 48  , 55 );
	fontdata[100] = Glyph(0, 717, 473 , 54  , 85  , 0   , 31  , 55 );
	fontdata[101] = Glyph(0, 781, 644 , 55  , 68  , 0   , 48  , 55 );
	fontdata[102] = Glyph(0, 188, 492 , 60  , 85  , 1   , 30  , 55 );
	fontdata[103] = Glyph(0, 640, 386 , 54  , 86  , 0   , 48  , 55 );
	fontdata[104] = Glyph(0, 343, 663 , 53  , 84  , 1   , 31  , 55 );
	fontdata[105] = Glyph(0, 618, 299 , 60  , 86  , 1   , 30  , 55 );
	fontdata[106] = Glyph(0, 632, 106 , 44  , 104 , 0   , 30  , 55 );
	fontdata[107] = Glyph(0, 613, 648 , 53  , 84  , 1   , 31  , 55 );
	fontdata[108] = Glyph(0, 249, 492 , 60  , 85  , 1   , 31  , 55 );
	fontdata[109] = Glyph(0, 160, 748 , 56  , 67  , 0   , 48  , 55 );
	fontdata[110] = Glyph(0, 271, 748 , 53  , 67  , 1   , 48  , 55 );
	fontdata[111] = Glyph(0, 893, 643 , 55  , 68  , 0   , 48  , 55 );
	fontdata[112] = Glyph(0, 695, 386 , 54  , 86  , 1   , 48  , 55 );
	fontdata[113] = Glyph(0, 555, 299 , 62  , 86  , 0   , 48  , 55 );
	fontdata[114] = Glyph(0, 325, 748 , 53  , 67  , 1   , 48  , 55 );
	fontdata[115] = Glyph(0, 108, 749 , 51  , 68  , 2   , 48  , 55 );
	fontdata[116] = Glyph(0, 310, 492 , 60  , 85  , 1   , 31  , 55 );
	fontdata[117] = Glyph(0, 217, 748 , 53  , 67  , 1   , 49  , 55 );
	fontdata[118] = Glyph(0, 445, 735 , 61  , 66  , -3  , 49  , 55 );
	fontdata[119] = Glyph(0, 379, 748 , 65  , 66  , -5  , 49  , 55 );
	fontdata[120] = Glyph(0, 507, 735 , 58  , 66  , -2  , 49  , 54 );
	fontdata[121] = Glyph(0, 772, 472 , 53  , 85  , 1   , 49  , 55 );
	fontdata[122] = Glyph(0, 566, 734 , 53  , 66  , 1   , 49  , 55 );
	fontdata[123] = Glyph(0, 194, 0   , 53  , 108 , 1   , 19  , 55 );
	fontdata[124] = Glyph(0, 456, 0   , 22  , 108 , 17  , 19  , 55 );
	fontdata[125] = Glyph(0, 248, 0   , 53  , 108 , 1   , 19  , 55 );
	fontdata[126] = Glyph(0, 803, 770 , 53  , 32  , 1   , 57  , 55 );
	fontdata[160] = Glyph(0, 1006, 87  , 16  , 15  , -7  , 122 , 55 );
	fontdata[161] = Glyph(0, 980, 0   , 27  , 86  , 14  , 48  , 55 );
	fontdata[162] = Glyph(0, 361, 214 , 53  , 89  , 0   , 38  , 55 );
	fontdata[163] = Glyph(0, 371, 479 , 58  , 85  , -4  , 30  , 55 );
	fontdata[164] = Glyph(0, 160, 816 , 53  , 53  , 1   , 46  , 55 );
	fontdata[165] = Glyph(0, 453, 565 , 61  , 84  , -3  , 31  , 55 );
	fontdata[166] = Glyph(0, 479, 0   , 22  , 108 , 17  , 19  , 55 );
	fontdata[167] = Glyph(0, 913, 385 , 53  , 86  , 1   , 30  , 55 );
	fontdata[168] = Glyph(0, 857, 769 , 42  , 27  , 6   , 30  , 55 );
	fontdata[169] = Glyph(0, 780, 713 , 56  , 56  , -1  , 30  , 55 );
	fontdata[170] = Glyph(0, 948, 712 , 49  , 54  , 3   , 30  , 55 );
	fontdata[171] = Glyph(0, 50 , 818 , 54  , 53  , -1  , 46  , 55 );
	fontdata[172] = Glyph(0, 473, 802 , 53  , 39  , 1   , 62  , 55 );
	fontdata[173] = Glyph(0, 54 , 872 , 53  , 22  , 1   , 62  , 55 );
	fontdata[174] = Glyph(0, 837, 712 , 56  , 56  , -1  , 30  , 55 );
	fontdata[175] = Glyph(0, 0  , 873 , 53  , 22  , 1   , 22  , 55 );
	fontdata[176] = Glyph(0, 527, 802 , 38  , 38  , 9   , 30  , 55 );
	fontdata[177] = Glyph(0, 620, 733 , 53  , 63  , 1   , 41  , 55 );
	fontdata[178] = Glyph(0, 267, 816 , 38  , 53  , 9   , 30  , 55 );
	fontdata[179] = Glyph(0, 983, 557 , 38  , 53  , 9   , 30  , 55 );
	fontdata[180] = Glyph(0, 691, 788 , 32  , 37  , 12  , 30  , 55 );
	fontdata[181] = Glyph(0, 826, 472 , 53  , 85  , 1   , 49  , 55 );
	fontdata[182] = Glyph(0, 287, 663 , 55  , 84  , -1  , 31  , 55 );
	fontdata[183] = Glyph(0, 984, 355 , 27  , 27  , 14  , 59  , 55 );
	fontdata[184] = Glyph(0, 446, 802 , 26  , 41  , 15  , 93  , 55 );
	fontdata[185] = Glyph(0, 360, 816 , 37  , 52  , 9   , 31  , 55 );
	fontdata[186] = Glyph(0, 0  , 818 , 49  , 54  , 3   , 30  , 55 );
	fontdata[187] = Glyph(0, 105, 818 , 54  , 53  , 2   , 46  , 55 );
	fontdata[188] = Glyph(0, 201, 578 , 64  , 84  , -5  , 31  , 55 );
	fontdata[189] = Glyph(0, 0  , 579 , 66  , 84  , -5  , 31  , 55 );
	fontdata[190] = Glyph(0, 123, 492 , 64  , 85  , -5  , 30  , 55 );
	fontdata[191] = Glyph(0, 470, 391 , 57  , 86  , -1  , 48  , 55 );
	fontdata[192] = Glyph(0, 913, 0   , 66  , 104 , -5  , 11  , 55 );
	fontdata[193] = Glyph(0, 67 , 109 , 66  , 104 , -5  , 11  , 55 );
	fontdata[194] = Glyph(0, 0  , 109 , 66  , 104 , -5  , 11  , 55 );
	fontdata[195] = Glyph(0, 67 , 214 , 66  , 102 , -5  , 13  , 55 );
	fontdata[196] = Glyph(0, 0  , 214 , 66  , 102 , -5  , 13  , 55 );
	fontdata[197] = Glyph(0, 677, 106 , 66  , 103 , -5  , 12  , 55 );
	fontdata[198] = Glyph(0, 950, 472 , 69  , 84  , -7  , 31  , 55 );
	fontdata[199] = Glyph(0, 196, 109 , 57  , 104 , -2  , 30  , 55 );
	fontdata[200] = Glyph(0, 362, 109 , 53  , 104 , 1   , 11  , 55 );
	fontdata[201] = Glyph(0, 578, 106 , 53  , 104 , 1   , 11  , 55 );
	fontdata[202] = Glyph(0, 524, 106 , 53  , 104 , 1   , 11  , 55 );
	fontdata[203] = Glyph(0, 191, 214 , 53  , 102 , 1   , 13  , 55 );
	fontdata[204] = Glyph(0, 470, 109 , 53  , 104 , 1   , 11  , 55 );
	fontdata[205] = Glyph(0, 416, 109 , 53  , 104 , 1   , 11  , 55 );
	fontdata[206] = Glyph(0, 308, 109 , 53  , 104 , 1   , 11  , 55 );
	fontdata[207] = Glyph(0, 245, 214 , 53  , 102 , 1   , 13  , 55 );
	fontdata[208] = Glyph(0, 266, 578 , 62  , 84  , -4  , 31  , 55 );
	fontdata[209] = Glyph(0, 134, 214 , 56  , 102 , -1  , 13  , 55 );
	fontdata[210] = Glyph(0, 624, 0   , 60  , 105 , -2  , 11  , 55 );
	fontdata[211] = Glyph(0, 563, 0   , 60  , 105 , -2  , 11  , 55 );
	fontdata[212] = Glyph(0, 502, 0   , 60  , 105 , -2  , 11  , 55 );
	fontdata[213] = Glyph(0, 744, 106 , 60  , 103 , -2  , 13  , 55 );
	fontdata[214] = Glyph(0, 805, 106 , 60  , 103 , -2  , 13  , 55 );
	fontdata[215] = Glyph(0, 398, 815 , 47  , 47  , 4   , 49  , 55 );
	fontdata[216] = Glyph(0, 740, 298 , 60  , 86  , -2  , 30  , 55 );
	fontdata[217] = Glyph(0, 801, 0   , 57  , 105 , -1  , 11  , 55 );
	fontdata[218] = Glyph(0, 743, 0   , 57  , 105 , -1  , 11  , 55 );
	fontdata[219] = Glyph(0, 685, 0   , 57  , 105 , -1  , 11  , 55 );
	fontdata[220] = Glyph(0, 866, 106 , 57  , 103 , -1  , 13  , 55 );
	fontdata[221] = Glyph(0, 134, 109 , 61  , 104 , -3  , 11  , 55 );
	fontdata[222] = Glyph(0, 635, 559 , 57  , 84  , -1  , 31  , 55 );
	fontdata[223] = Glyph(0, 750, 385 , 54  , 86  , 2   , 30  , 55 );
	fontdata[224] = Glyph(0, 112, 317 , 53  , 87  , 1   , 29  , 55 );
	fontdata[225] = Glyph(0, 274, 317 , 53  , 87  , 1   , 29  , 55 );
	fontdata[226] = Glyph(0, 220, 317 , 53  , 87  , 1   , 29  , 55 );
	fontdata[227] = Glyph(0, 166, 317 , 53  , 87  , 1   , 29  , 55 );
	fontdata[228] = Glyph(0, 967, 385 , 53  , 86  , 1   , 30  , 55 );
	fontdata[229] = Glyph(0, 476, 214 , 53  , 88  , 1   , 28  , 55 );
	fontdata[230] = Glyph(0, 718, 644 , 62  , 68  , -4  , 48  , 55 );
	fontdata[231] = Glyph(0, 0  , 492 , 53  , 86  , 0   , 48  , 55 );
	fontdata[232] = Glyph(0, 713, 210 , 55  , 87  , 0   , 29  , 55 );
	fontdata[233] = Glyph(0, 881, 210 , 55  , 87  , 0   , 29  , 55 );
	fontdata[234] = Glyph(0, 825, 210 , 55  , 87  , 0   , 29  , 55 );
	fontdata[235] = Glyph(0, 584, 386 , 55  , 86  , 0   , 30  , 55 );
	fontdata[236] = Glyph(0, 530, 211 , 60  , 87  , 1   , 29  , 55 );
	fontdata[237] = Glyph(0, 591, 211 , 60  , 87  , 1   , 29  , 55 );
	fontdata[238] = Glyph(0, 652, 211 , 60  , 87  , 1   , 29  , 55 );
	fontdata[239] = Glyph(0, 0  , 405 , 59  , 86  , 1   , 30  , 55 );
	fontdata[240] = Glyph(0, 415, 214 , 60  , 88  , -1  , 28  , 56 );
	fontdata[241] = Glyph(0, 859, 385 , 53  , 86  , 1   , 29  , 55 );
	fontdata[242] = Glyph(0, 56 , 317 , 55  , 87  , 0   , 29  , 55 );
	fontdata[243] = Glyph(0, 0  , 317 , 55  , 87  , 0   , 29  , 55 );
	fontdata[244] = Glyph(0, 769, 210 , 55  , 87  , 0   , 29  , 55 );
	fontdata[245] = Glyph(0, 937, 209 , 55  , 87  , 0   , 29  , 55 );
	fontdata[246] = Glyph(0, 528, 390 , 55  , 86  , 0   , 30  , 55 );
	fontdata[247] = Glyph(0, 894, 712 , 53  , 56  , 1   , 45  , 55 );
	fontdata[248] = Glyph(0, 837, 643 , 55  , 68  , 0   , 48  , 55 );
	fontdata[249] = Glyph(0, 436, 303 , 53  , 87  , 1   , 29  , 55 );
	fontdata[250] = Glyph(0, 382, 304 , 53  , 87  , 1   , 29  , 55 );
	fontdata[251] = Glyph(0, 328, 304 , 53  , 87  , 1   , 29  , 55 );
	fontdata[252] = Glyph(0, 805, 385 , 53  , 86  , 1   , 30  , 55 );
	fontdata[253] = Glyph(0, 859, 0   , 53  , 105 , 1   , 29  , 55 );
	fontdata[254] = Glyph(0, 924, 105 , 53  , 103 , 2   , 31  , 55 );
	fontdata[255] = Glyph(0, 254, 109 , 53  , 104 , 1   , 30  , 55 );
	for(int i=0; i<256; ++i) {
		if(fontdata[i].id != -1)
			fontdata[i].id = i;
	}

	return fontdata;
}

FontData Fonts::setFontConsolaMono() {
	FontData data;
	setFontConsolaMono(data);
	return data;
}
