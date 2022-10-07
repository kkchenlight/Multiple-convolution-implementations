/*
 * @Description:
 * @Author: kkchen
 * @Email: kkchen.lg@qq.com
 * @Date: 2022-10-02 13:57:33
 * @LastEditTime: 2022-10-06 11:43:23
 * @LastEditors: kkchen
 */
#include <time.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "./util/common.h"

void BaseConv(float* src_, float* dst_, float* weight_, Shape src_shape_,
              Shape dst_shape_) {
  for (int j = 0; j < dst_shape_.h_; j++) {
    for (int i = 0; i < dst_shape_.w_; i++) {
      float sum = 0;
      for (int w_j = 0; w_j < 3; w_j++) {
        for (int w_i = 0; w_i < 3; w_i++) {
          int src_j_index = j + w_j;
          int src_i_index = i + w_i;
          sum += weight_[w_j * 3 + w_i] *
                 src_[src_j_index * src_shape_.w_ + src_i_index];
        }
      }
      dst_[j * dst_shape_.w_ + i] = sum;
    }
  }
}

void GetQuant(float* src_, int8_t* quant_, Shape src_shape_,
              QuantParam& quant_param_) {
  int len = src_shape_.GetArea();
  quant_param_.max_value_ = 0.0;
  for (int i = 0; i < len; i++) {
    if (std::abs(src_[i]) > quant_param_.max_value_) {
      quant_param_.max_value_ = std::abs(src_[i]);
    }
  }
  quant_param_.scale_ = quant_param_.max_value_ / 127;
  quant_param_.zero_ = 0;

  for (int i = 0; i < len; i++) {
    quant_[i] = (int8_t)(src_[i] / quant_param_.scale_);
  }
}

void CheckQuantResult(float* src_, int8_t* quant_src_, Shape src_shape_,
                      QuantParam& quant_param_) {
  int len = src_shape_.GetArea();
  float mae = 0;
  for (int i = 0; i < len; i++) {
    mae += std::abs(src_[i] - quant_src_[i] * quant_param_.scale_);
  }
  std::cout << "mae is : " << mae / src_shape_.GetArea() << std::endl;
}

void QuantConv(int8_t* src_, int8_t* weight_, float* dst_, Shape src_shape_,
               Shape dst_shape_, QuantParam src_quant_param_,
               QuantParam weight_quant_param_) {
  float merge_scale_ = src_quant_param_.scale_ * weight_quant_param_.scale_;
  for (int j = 0; j < dst_shape_.h_; j++) {
    for (int i = 0; i < dst_shape_.w_; i++) {
      int32_t sum = 0;
      for (int w_j = 0; w_j < 3; w_j++) {
        for (int w_i = 0; w_i < 3; w_i++) {
          int src_j_index = j + w_j;
          int src_i_index = i + w_i;
        }
      }
      dst_[j * dst_shape_.w_ + i] = sum * merge_scale_;
    }
  }
}

template <class T>
void CalculateMae(T* src1_, T* src2_, int len) {
  T sum = 0.0;
  for (int i = 0; i < len; i++) {
    sum += std::abs(src1_[i] - src2_[i]);
  }
  std::cout << "mae = " << sum << std::endl;
}

void GenerateRandomData(float* data_, Shape shape_) {
  std::uniform_real_distribution<double> u(-1, 1);
  std::default_random_engine e(time(NULL));
  for (int j = 0; j < shape_.h_; j++) {
    for (int i = 0; i < shape_.w_; i++) {
      data_[j * shape_.w_ + i] = u(e);
    }
  }
}

int main(int argc, char** argv) {
  /*PrePare the weight*/
  Shape weight_shape_(3, 3);
  float* weight_ = new float[weight_shape_.GetArea()];
  GenerateRandomData(weight_, weight_shape_);

  /*Get src data*/
  Shape src_shape_(4, 4);
  Shape dst_shape_(2, 2);
  float* src_ = new float(src_shape_.GetArea());
  float* dst_ = new float(dst_shape_.GetArea());
  GenerateRandomData(src_, src_shape_);

  /*getQuant*/
  QuantParam src_quant_param_;
  QuantParam weight_quant_param_;

  int8_t* src_quant_ = new int8_t[src_shape_.GetArea()];
  int8_t* weight_quant_ = new int8_t[weight_shape_.GetArea()];
  float* dst_quant_ = new float[dst_shape_.GetArea()];
  //   std::cout << " >>>>>>>>>>> " << std::endl;
  //   for (int i = 0; i < src_shape_.GetArea(); i++) {
  //     std::cout << " src = " << src_[i] << std::endl;
  //   }
  GetQuant(src_, src_quant_, src_shape_, src_quant_param_);
  GetQuant(weight_, weight_quant_, weight_shape_, weight_quant_param_);
  CheckQuantResult(src_, src_quant_, src_shape_, src_quant_param_);
  CheckQuantResult(weight_, weight_quant_, weight_shape_, weight_quant_param_);

  {
    std::string test_name = "test";
    Time t = Time(test_name);
    for (int i = 0; i < 100; i++) {
      BaseConv(src_, dst_, weight_, src_shape_, dst_shape_);
    }
  }

  {
    std::string test_name = "test";
    Time t = Time(test_name);
    for (int i = 0; i < 1000; i++) {
      QuantConv(src_quant_, weight_quant_, dst_quant_, src_shape_, dst_shape_,
                src_quant_param_, weight_quant_param_);
    }
  }

  /*winograd convolution*/
  WinogradConv Conv = WinogradConv(weight_);
  Conv.Preforward();
  float* winograd_conv_dst_ = new float[dst_shape_.GetArea()];
  {
    std::string test_name = "winograd";
    Time t = Time(test_name);
    for (int i = 0; i < 1000; i++) {
      Conv.Forward(src_, winograd_conv_dst_);
    }
  }

  CalculateMae<float>(dst_, winograd_conv_dst_, dst_shape_.GetArea());
  CalculateMae<float>(dst_, dst_quant_, dst_shape_.GetArea());

  std::cout << "hello world" << std::endl;
}
