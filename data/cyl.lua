cyl = gr.node('cyl')
cyl_body = gr.mesh('cyl_body', {
  {0, 1, 0},
  {0, -1, 0},
  {0.0, 1, 1.0},
  {0.0, -1, 1.0},
  {0.207911690818, 1, 0.978147600734},
  {0.207911690818, -1, 0.978147600734},
  {0.406736643076, 1, 0.913545457643},
  {0.406736643076, -1, 0.913545457643},
  {0.587785252292, 1, 0.809016994375},
  {0.587785252292, -1, 0.809016994375},
  {0.743144825477, 1, 0.669130606359},
  {0.743144825477, -1, 0.669130606359},
  {0.866025403784, 1, 0.5},
  {0.866025403784, -1, 0.5},
  {0.951056516295, 1, 0.309016994375},
  {0.951056516295, -1, 0.309016994375},
  {0.994521895368, 1, 0.104528463268},
  {0.994521895368, -1, 0.104528463268},
  {0.994521895368, 1, -0.104528463268},
  {0.994521895368, -1, -0.104528463268},
  {0.951056516295, 1, -0.309016994375},
  {0.951056516295, -1, -0.309016994375},
  {0.866025403784, 1, -0.5},
  {0.866025403784, -1, -0.5},
  {0.743144825477, 1, -0.669130606359},
  {0.743144825477, -1, -0.669130606359},
  {0.587785252292, 1, -0.809016994375},
  {0.587785252292, -1, -0.809016994375},
  {0.406736643076, 1, -0.913545457643},
  {0.406736643076, -1, -0.913545457643},
  {0.207911690818, 1, -0.978147600734},
  {0.207911690818, -1, -0.978147600734},
  {5.66549845232e-16, 1, -1.0},
  {5.66549845232e-16, -1, -1.0},
  {-0.207911690818, 1, -0.978147600734},
  {-0.207911690818, -1, -0.978147600734},
  {-0.406736643076, 1, -0.913545457643},
  {-0.406736643076, -1, -0.913545457643},
  {-0.587785252292, 1, -0.809016994375},
  {-0.587785252292, -1, -0.809016994375},
  {-0.743144825477, 1, -0.669130606359},
  {-0.743144825477, -1, -0.669130606359},
  {-0.866025403784, 1, -0.5},
  {-0.866025403784, -1, -0.5},
  {-0.951056516295, 1, -0.309016994375},
  {-0.951056516295, -1, -0.309016994375},
  {-0.994521895368, 1, -0.104528463268},
  {-0.994521895368, -1, -0.104528463268},
  {-0.994521895368, 1, 0.104528463268},
  {-0.994521895368, -1, 0.104528463268},
  {-0.951056516295, 1, 0.309016994375},
  {-0.951056516295, -1, 0.309016994375},
  {-0.866025403784, 1, 0.5},
  {-0.866025403784, -1, 0.5},
  {-0.743144825477, 1, 0.669130606359},
  {-0.743144825477, -1, 0.669130606359},
  {-0.587785252292, 1, 0.809016994375},
  {-0.587785252292, -1, 0.809016994375},
  {-0.406736643076, 1, 0.913545457643},
  {-0.406736643076, -1, 0.913545457643},
  {-0.207911690818, 1, 0.978147600734},
  {-0.207911690818, -1, 0.978147600734},
  {-1.13309969046e-15, 1, 1.0},
  {-1.13309969046e-15, -1, 1.0}}, {
  {2, 3, 5, 4},
  {2, 4, 4, 2},
  {3, 5, 5, 3},
  {4, 5, 7, 6},
  {4, 6, 6, 4},
  {5, 7, 7, 5},
  {6, 7, 9, 8},
  {6, 8, 8, 6},
  {7, 9, 9, 7},
  {8, 9, 11, 10},
  {8, 10, 10, 8},
  {9, 11, 11, 9},
  {10, 11, 13, 12},
  {10, 12, 12, 10},
  {11, 13, 13, 11},
  {12, 13, 15, 14},
  {12, 14, 14, 12},
  {13, 15, 15, 13},
  {14, 15, 17, 16},
  {14, 16, 16, 14},
  {15, 17, 17, 15},
  {16, 17, 19, 18},
  {16, 18, 18, 16},
  {17, 19, 19, 17},
  {18, 19, 21, 20},
  {18, 20, 20, 18},
  {19, 21, 21, 19},
  {20, 21, 23, 22},
  {20, 22, 22, 20},
  {21, 23, 23, 21},
  {22, 23, 25, 24},
  {22, 24, 24, 22},
  {23, 25, 25, 23},
  {24, 25, 27, 26},
  {24, 26, 26, 24},
  {25, 27, 27, 25},
  {26, 27, 29, 28},
  {26, 28, 28, 26},
  {27, 29, 29, 27},
  {28, 29, 31, 30},
  {28, 30, 30, 28},
  {29, 31, 31, 29},
  {30, 31, 33, 32},
  {30, 32, 32, 30},
  {31, 33, 33, 31},
  {32, 33, 35, 34},
  {32, 34, 34, 32},
  {33, 35, 35, 33},
  {34, 35, 37, 36},
  {34, 36, 36, 34},
  {35, 37, 37, 35},
  {36, 37, 39, 38},
  {36, 38, 38, 36},
  {37, 39, 39, 37},
  {38, 39, 41, 40},
  {38, 40, 40, 38},
  {39, 41, 41, 39},
  {40, 41, 43, 42},
  {40, 42, 42, 40},
  {41, 43, 43, 41},
  {42, 43, 45, 44},
  {42, 44, 44, 42},
  {43, 45, 45, 43},
  {44, 45, 47, 46},
  {44, 46, 46, 44},
  {45, 47, 47, 45},
  {46, 47, 49, 48},
  {46, 48, 48, 46},
  {47, 49, 49, 47},
  {48, 49, 51, 50},
  {48, 50, 50, 48},
  {49, 51, 51, 49},
  {50, 51, 53, 52},
  {50, 52, 52, 50},
  {51, 53, 53, 51},
  {52, 53, 55, 54},
  {52, 54, 54, 52},
  {53, 55, 55, 53},
  {54, 55, 57, 56},
  {54, 56, 56, 54},
  {55, 57, 57, 55},
  {56, 57, 59, 58},
  {56, 58, 58, 56},
  {57, 59, 59, 57},
  {58, 59, 61, 60},
  {58, 60, 60, 58},
  {59, 61, 61, 59},
  {60, 61, 63, 62},
  {60, 62, 62, 60},
  {61, 63, 63, 61}})
cyl:add_child(cyl_body)
cyl_ends = gr.mesh('cyl_ends', {
  {0, 1, 0},
  {0, -1, 0},
  {0.0, 1, 1.0},
  {0.0, -1, 1.0},
  {0.207911690818, 1, 0.978147600734},
  {0.207911690818, -1, 0.978147600734},
  {0.406736643076, 1, 0.913545457643},
  {0.406736643076, -1, 0.913545457643},
  {0.587785252292, 1, 0.809016994375},
  {0.587785252292, -1, 0.809016994375},
  {0.743144825477, 1, 0.669130606359},
  {0.743144825477, -1, 0.669130606359},
  {0.866025403784, 1, 0.5},
  {0.866025403784, -1, 0.5},
  {0.951056516295, 1, 0.309016994375},
  {0.951056516295, -1, 0.309016994375},
  {0.994521895368, 1, 0.104528463268},
  {0.994521895368, -1, 0.104528463268},
  {0.994521895368, 1, -0.104528463268},
  {0.994521895368, -1, -0.104528463268},
  {0.951056516295, 1, -0.309016994375},
  {0.951056516295, -1, -0.309016994375},
  {0.866025403784, 1, -0.5},
  {0.866025403784, -1, -0.5},
  {0.743144825477, 1, -0.669130606359},
  {0.743144825477, -1, -0.669130606359},
  {0.587785252292, 1, -0.809016994375},
  {0.587785252292, -1, -0.809016994375},
  {0.406736643076, 1, -0.913545457643},
  {0.406736643076, -1, -0.913545457643},
  {0.207911690818, 1, -0.978147600734},
  {0.207911690818, -1, -0.978147600734},
  {5.66549845232e-16, 1, -1.0},
  {5.66549845232e-16, -1, -1.0},
  {-0.207911690818, 1, -0.978147600734},
  {-0.207911690818, -1, -0.978147600734},
  {-0.406736643076, 1, -0.913545457643},
  {-0.406736643076, -1, -0.913545457643},
  {-0.587785252292, 1, -0.809016994375},
  {-0.587785252292, -1, -0.809016994375},
  {-0.743144825477, 1, -0.669130606359},
  {-0.743144825477, -1, -0.669130606359},
  {-0.866025403784, 1, -0.5},
  {-0.866025403784, -1, -0.5},
  {-0.951056516295, 1, -0.309016994375},
  {-0.951056516295, -1, -0.309016994375},
  {-0.994521895368, 1, -0.104528463268},
  {-0.994521895368, -1, -0.104528463268},
  {-0.994521895368, 1, 0.104528463268},
  {-0.994521895368, -1, 0.104528463268},
  {-0.951056516295, 1, 0.309016994375},
  {-0.951056516295, -1, 0.309016994375},
  {-0.866025403784, 1, 0.5},
  {-0.866025403784, -1, 0.5},
  {-0.743144825477, 1, 0.669130606359},
  {-0.743144825477, -1, 0.669130606359},
  {-0.587785252292, 1, 0.809016994375},
  {-0.587785252292, -1, 0.809016994375},
  {-0.406736643076, 1, 0.913545457643},
  {-0.406736643076, -1, 0.913545457643},
  {-0.207911690818, 1, 0.978147600734},
  {-0.207911690818, -1, 0.978147600734},
  {-1.13309969046e-15, 1, 1.0},
  {-1.13309969046e-15, -1, 1.0}}, {
  {2, 4, 0, 0},
  {1, 1, 5, 3},
  {4, 6, 0, 0},
  {1, 1, 7, 5},
  {6, 8, 0, 0},
  {1, 1, 9, 7},
  {8, 10, 0, 0},
  {1, 1, 11, 9},
  {10, 12, 0, 0},
  {1, 1, 13, 11},
  {12, 14, 0, 0},
  {1, 1, 15, 13},
  {14, 16, 0, 0},
  {1, 1, 17, 15},
  {16, 18, 0, 0},
  {1, 1, 19, 17},
  {18, 20, 0, 0},
  {1, 1, 21, 19},
  {20, 22, 0, 0},
  {1, 1, 23, 21},
  {22, 24, 0, 0},
  {1, 1, 25, 23},
  {24, 26, 0, 0},
  {1, 1, 27, 25},
  {26, 28, 0, 0},
  {1, 1, 29, 27},
  {28, 30, 0, 0},
  {1, 1, 31, 29},
  {30, 32, 0, 0},
  {1, 1, 33, 31},
  {32, 34, 0, 0},
  {1, 1, 35, 33},
  {34, 36, 0, 0},
  {1, 1, 37, 35},
  {36, 38, 0, 0},
  {1, 1, 39, 37},
  {38, 40, 0, 0},
  {1, 1, 41, 39},
  {40, 42, 0, 0},
  {1, 1, 43, 41},
  {42, 44, 0, 0},
  {1, 1, 45, 43},
  {44, 46, 0, 0},
  {1, 1, 47, 45},
  {46, 48, 0, 0},
  {1, 1, 49, 47},
  {48, 50, 0, 0},
  {1, 1, 51, 49},
  {50, 52, 0, 0},
  {1, 1, 53, 51},
  {52, 54, 0, 0},
  {1, 1, 55, 53},
  {54, 56, 0, 0},
  {1, 1, 57, 55},
  {56, 58, 0, 0},
  {1, 1, 59, 57},
  {58, 60, 0, 0},
  {1, 1, 61, 59},
  {60, 62, 0, 0},
  {1, 1, 63, 61}})
--cyl_ends:no_outline()
cyl:add_child(cyl_ends)
