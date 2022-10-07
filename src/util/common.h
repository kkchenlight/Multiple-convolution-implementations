/*
 * @Description:
 * @Author: kkchen
 * @Email: kkchen.lg@qq.com
 * @Date: 2022-10-02 14:31:24
 * @LastEditTime: 2022-10-06 11:43:52
 * @LastEditors: kkchen
 */
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

class Time {
 public:
  Time(std::string& test_name) {
    test_name_ = test_name;
    start = std::chrono::steady_clock::now();
  }
  ~Time() {
    auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    std::cout << test_name_ << " cost " << duration_.count()
              << " micro seconds!!" << std::endl;
  }
  std::string test_name_;
  std::chrono::steady_clock::time_point start;
};

struct Shape {
  int w_;
  int h_;
  Shape(int w, int h) : w_(w), h_(h){};
  ~Shape(){};
  int GetArea() { return w_ * h_; };
};

void Mul(float* src1_, float* src2_, float* dst_, Shape src_shape_) {
  for (int j = 0; j < src_shape_.h_; j++) {
    for (int i = 0; i < src_shape_.w_; i++) {
      dst_[j * src_shape_.w_ + i] =
          src1_[j * src_shape_.w_ + i] * src2_[j * src_shape_.w_ + i];
    }
  }
}

void PrintShape(Shape shape_) {
  std::cout << " <<<<<<<<<<< shape print >>>>>>>>>>>>>>" << std::endl;
  std::string shape_str_ = "w_h:";
  shape_str_ += "[" + std::to_string(shape_.w_);
  shape_str_ += "," + std::to_string(shape_.h_) + "]";
  std::cout << shape_str_ << std::endl;
  return;
}
void Dot(float* src1_, float* src2_, float* dst_, Shape src1_shape_,
         Shape src2_shape_) {
  for (int j = 0; j < src1_shape_.h_; j++) {
    for (int i = 0; i < src2_shape_.w_; i++) {
      float sum = 0;
      for (int p = 0; p < src1_shape_.w_; p++) {
        sum +=
            (src1_[j * src1_shape_.w_ + p]) * (src2_[p * src2_shape_.w_ + i]);
      }
      dst_[j * src2_shape_.w_ + i] = sum;
    }
  }
}
// using Vec2D = std::vector<std::vector<float>>;

class WinogradConv {
 public:
  // Vec2D A_ = {{1, 0}, {1, 1}, {1, -1}, {0, -1}};
  // Vec2D A_t_ = {{1, 1, 1, 0}, {0, 1, -1, -1}};
  // Vec2D B_ = {{1, 0, 0, 0}, {0, 1, -1, 1}, {-1, 1, 1, 0}, {0, 0, 0, -1}};
  // Vec2D B_t_ = {{1, 0, -1, 0}, {0, 1, 1, 0}, {0, -1, 1, 0}, {0, 1, 0, -1}};
  float G_[12] = {1, 0, 0, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0, 0, 1};
  float G_t_[12] = {1, 0.5, 0.5, 0, 0, 0.5, -0.5, 0, 0, 0.5, 0.5, 1};
  float* weight_;
  float* weight_transform_;
  Shape weight_shape_ = Shape(3, 3);

  WinogradConv(float* weight_bake_) {
    weight_transform_ = new float[12];
    weight_ = weight_bake_;
  };
  ~WinogradConv(){};

  void CaculateBtSrc_(float* src_a_, float* Bt_src_) {
    Bt_src_[0] = src_a_[0] - src_a_[8];
    Bt_src_[1] = src_a_[1] - src_a_[9];
    Bt_src_[2] = src_a_[2] - src_a_[10];
    Bt_src_[3] = src_a_[3] - src_a_[11];

    Bt_src_[4] = src_a_[4] + src_a_[8];
    Bt_src_[5] = src_a_[5] + src_a_[9];
    Bt_src_[6] = src_a_[6] + src_a_[10];
    Bt_src_[7] = src_a_[7] + src_a_[11];

    Bt_src_[8] = -src_a_[4] + src_a_[8];
    Bt_src_[9] = -src_a_[5] + src_a_[9];
    Bt_src_[10] = -src_a_[6] + src_a_[10];
    Bt_src_[11] = -src_a_[7] + src_a_[11];

    Bt_src_[12] = -src_a_[4] - src_a_[12];
    Bt_src_[13] = -src_a_[5] + src_a_[13];
    Bt_src_[14] = -src_a_[6] + src_a_[14];
    Bt_src_[15] = -src_a_[7] + src_a_[15];
  }

  void CaculateBtSrcB_(float* src_b_, float* V_) {
    V_[0] = src_b_[0] - src_b_[2];
    V_[4] = src_b_[4] - src_b_[6];
    V_[8] = src_b_[8] - src_b_[10];
    V_[12] = src_b_[12] - src_b_[14];

    V_[1] = src_b_[1] + src_b_[2];
    V_[5] = src_b_[5] + src_b_[6];
    V_[9] = src_b_[9] + src_b_[10];
    V_[13] = src_b_[13] + src_b_[14];

    V_[2] = -src_b_[1] + src_b_[2];
    V_[6] = -src_b_[5] + src_b_[6];
    V_[10] = -src_b_[9] + src_b_[10];
    V_[14] = -src_b_[13] + src_b_[14];

    V_[3] = src_b_[1] - src_b_[3];
    V_[7] = src_b_[5] - src_b_[7];
    V_[11] = src_b_[9] - src_b_[11];
    V_[15] = src_b_[13] - src_b_[15];
  }

  void CaculateUMulV(float* V_, float* U_, float* UV_) {
    Mul(U_, V_, UV_, Shape(1, 16));
  };

  void CaculateAtUV(float* UV_, float* AtUV_) {
    AtUV_[0] = UV_[0] + UV_[4] + UV_[8];
    AtUV_[1] = UV_[1] + UV_[5] + UV_[9];
    AtUV_[2] = UV_[2] + UV_[6] + UV_[10];
    AtUV_[3] = UV_[3] + UV_[7] + UV_[11];

    AtUV_[4] = UV_[4] - UV_[8] - UV_[12];
    AtUV_[5] = UV_[5] - UV_[9] - UV_[13];
    AtUV_[6] = UV_[6] - UV_[10] - UV_[14];
    AtUV_[7] = UV_[7] - UV_[11] - UV_[15];
  }

  void CaculateUVA(float* result, float* AtUV_) {
    result[0] += (AtUV_[0] + AtUV_[1] + AtUV_[2]);
    result[2] += (AtUV_[4] + AtUV_[5] + AtUV_[6]);
    result[1] += (AtUV_[1] - AtUV_[2] - AtUV_[3]);
    result[3] += (AtUV_[5] - AtUV_[6] - AtUV_[7]);
  }

  void Forward(float* src_, float* dst_) {
    float Bt_src_[16] = {0};
    float V_[16] = {0};
    float UV_[16] = {0};
    float At_UV_[16] = {0};

    CaculateBtSrc_(src_, Bt_src_);
    CaculateBtSrcB_(Bt_src_, V_);
    CaculateUMulV(V_, weight_transform_, UV_);
    CaculateAtUV(UV_, At_UV_);
    CaculateUVA(dst_, At_UV_);
  };
  void Preforward() {
    float Gg[12] = {0};
    Dot(G_, weight_, Gg, Shape(3, 4), Shape(3, 3));
    Dot(Gg, G_t_, weight_transform_, Shape(3, 4), Shape(4, 3));
  }
};

struct QuantParam {
  /* data */
  float scale_;
  float zero_;
  float max_value_;
};

template <class T>
void PrintMatrix(Shape shape_, T* src_) {
  for (int j = 0; j < shape_.h_; j++) {
    std::cout << std::endl;
    for (int i = 0; i < shape_.w_; i++) {
      std::cout << " " << (float)(src_[j * shape_.w_ + j]);
    }
  }
  std::cout << std::endl << " print Matrix >>>>>>>>>>>>>> " << std::endl;
}
