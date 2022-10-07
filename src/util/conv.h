/*
 * @Description: 
 * @Author: kkchen
 * @Email: kkchen.lg@qq.com
 * @Date: 2022-10-02 14:28:55
 * @LastEditTime: 2022-10-02 14:48:45
 * @LastEditors: kkchen
 */
#include<iostream>
#include<string>
#include<vector>

#include "common.h"


class conv
{
private:
    /* data */
    float* in_data_;
    float* weight_;
    float* bias_;
    bool bias_term_;
    float* out_data_;

    /* shape */
    shape x_shape_;
    shape weight_shape_;
    shape bias_shape_;
    shape y_shape_;

    /*index*/
    std::vector<std::vector<std::pair<int, int>>> index_int_;

    
public:
    conv(/* args */);
    ~conv();
    run();
};



