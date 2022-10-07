/*
 * @Description:
 * @Author: kkchen
 * @Email: kkchen.lg@qq.com
 * @Date: 2022-10-02 14:35:27
 * @LastEditTime: 2022-10-06 11:44:37
 * @LastEditors: kkchen
 */
#include "conv.h"

conv::conv() {}

conv::~conv() {}

conv::run() {
  for (int i = 0; i < y_shape_.h; i++) {
    for (int j = 0; j < y_shape_.w; j++) {
      float sum = 0.0;
      for (int weight_i = 0; weight_i < 3; weight_i++) {
        for (int weight_j = 0; weight_j < 3; weight_j++) {
          int input_i = i + weight_i;
          int intput_j = j + weight_j;
          sum += in_data_[input_i * x_shape_.w + input_j]*
        }
      }
    }
  }
}
