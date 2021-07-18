#ifndef HEX_PADDING_UTILITY_H
#define HEX_PADDING_UTILITY_H

/* vertexes order in this table ensures that normal vector of faces points into the cell */
const unsigned int HexFace[6][4] =
{
    { 0,1,2,3 },
    { 4,7,6,5 },
    { 0,4,5,1 },
    { 0,3,7,4 },
    { 3,2,6,7 },
    { 1,5,6,2 },
};

#endif
