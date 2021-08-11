#ifndef MU_COMMON_H
#define MU_COMMON_H

typedef float vec_t;

typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec34_t[3][4];


#define ITEM_CATEGORY_MAX 21	//(0-20)	
#define ITEM_INDEX_MAX 512		//(0-511)
#define ITEM_MAKE(cat, idx) (cat * 512 + idx)

#endif // !MU_COMMON_H
