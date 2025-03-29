#ifndef PDI_HPP
#define PDI_HPP

#include <PDI/core.hpp>
#include <PDI/theme.hpp>
#include <PDI/components.hpp>
#include <PDI/math.hpp>

#include <GLUI/glui.hpp>




void __simd_kernel_multiplication( mat* r, uint8_t* in, uint8_t* nout, Size* sz, uint8_t ch );
void __isimd_kernel_multiplication( mat* r, uint8_t* in, uint8_t* nout, Size* sz, uint8_t ch );
void __dflt_kernel_multiplication( mat* r, uint8_t* in, uint8_t* nout, Size* sz, uint8_t ch );

// void align_input_data(uint8_t* stb_image_data, size_t total_pixels, uint8_t*& aligned_in) {
//     size_t alignment = 32; 

//     if (posix_memalign((void**)&aligned_in, alignment, total_pixels * sizeof(uint8_t)) != 0) {
//         std::cerr << "Memory allocation for aligned input failed!" << std::endl;
//         exit(1);
//     }

//     std::memcpy(aligned_in, stb_image_data, total_pixels * sizeof(uint8_t));
// }

#endif