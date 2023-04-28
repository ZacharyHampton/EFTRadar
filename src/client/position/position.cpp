#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdint.h>
#include <windows.h>

struct TransformAccessReadOnly
{
    uint64_t	pTransformData;
    int			index;
};

struct TransformData
{
    uint64_t pTransformArray;
    uint64_t pTransformIndices;
};

struct Matrix34
{
    Vector4 vec0;
    Vector4 vec1;
    Vector4 vec2;
};

struct Vector3
{
    float x;
    float y;
    float z;
};

struct Vector4
{
    float w;
    float x;
    float y;
    float z;
};


PyObject* GetPosition(PyObject* self, PyObject* args)
{
    __m128 result;

    const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
    const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
    const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

    // TransformAccessReadOnly pTransformAccessReadOnly = Driver.ReadMem<TransformAccessReadOnly>(gTargetProcessID, (ULONGLONG)this + 0x38);

    // TransformData transformData = Driver.ReadMem<TransformData>(gTargetProcessID, pTransformAccessReadOnly.pTransformData + 0x8);

    SIZE_T sizeMatriciesBuf = sizeof(Matrix34) * pTransformAccessReadOnly.index + sizeof(Matrix34);
    SIZE_T sizeIndicesBuf = sizeof(int) * pTransformAccessReadOnly.index + sizeof(int);

    // Allocate memory for storing large amounts of data (matricies and indicies)
    PVOID pMatriciesBuf = malloc(sizeMatriciesBuf);
    PVOID pIndicesBuf = malloc(sizeIndicesBuf);

    if(pMatriciesBuf && pIndicesBuf)
    {
        // Read Matricies array into the buffer
        // Driver.ReadMem(gTargetProcessID, transformData.pTransformArray, pMatriciesBuf, sizeMatriciesBuf);
        // Read Indices array into the buffer
        // Driver.ReadMem(gTargetProcessID, transformData.pTransformIndices, pIndicesBuf, sizeIndicesBuf);

        result = *(__m128*)((ULONGLONG)pMatriciesBuf + 0x30 * pTransformAccessReadOnly.index);
        int transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * pTransformAccessReadOnly.index);

        while(transformIndex >= 0)
        {
            Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);

            __m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
            __m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
            __m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
            __m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
            __m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
            __m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
            __m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

            result = _mm_add_ps(
                    _mm_add_ps(
                            _mm_add_ps(
                                    _mm_mul_ps(
                                            _mm_sub_ps(
                                                    _mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
                                                    _mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
                                            _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
                                    _mm_mul_ps(
                                            _mm_sub_ps(
                                                    _mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
                                                    _mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
                                            _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
                            _mm_add_ps(
                                    _mm_mul_ps(
                                            _mm_sub_ps(
                                                    _mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
                                                    _mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
                                            _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
                                    tmp7)), *(__m128*)(&matrix34.vec0));

            transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * transformIndex);
        }

        free(pMatriciesBuf);
        free(pIndicesBuf);
    }

    return Py_BuildValue("(fff)", result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}