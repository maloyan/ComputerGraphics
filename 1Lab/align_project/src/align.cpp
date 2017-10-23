#include "align.h"
#include <string>

using namespace std;

#define RANGE 15
#define BORDER 13
#define M_PI 3.14159265358979323846

struct shiftCord
{
    int shift_row = 0;
    int shift_col = 0;
};

uint idxCheck(double pixel, int a, int b) 
{
    if (pixel < a) {
        return a;
    } else if (pixel > b) {
        return b;
    } else {
        return static_cast<uint>(pixel);
    }
}

double meanSqError(Image I1, Image I2) 
{
    double ans = 0;
    for (uint i = BORDER; i < I1.n_rows - BORDER; i++) {
        for (uint j = BORDER; j < I1.n_cols - BORDER; j++) {
            ans += (get<0>(I1(i, j)) - get<0>(I2(i, j))) * (get<0>(I1(i, j)) - get<0>(I2(i, j)));
        }
    }
    ans = ans / (I1.n_rows * I1.n_cols);
    return ans; 
}

double crossCorrelation(Image I1, Image I2)
{
    double ans = 0;
    uint hight = I1.n_rows, width = I1.n_cols;
    for (uint i = 0; i < hight; i++) {
        for (uint j = 0; j < width; j++) {
            ans += get<0>(I1(i, j)) * get<0>(I2(i, j));
        }
    }
    return ans;
}

shiftCord imageShift(Image I1, Image I2, int range)
{
    double checker = 0, tmp = 0;
    shiftCord shift;
    Image fixedImage, indentedImage;

    fixedImage = I1.submatrix(range, range, 
                              I1.n_rows - 2 * range, I1.n_cols - 2 * range);    
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {

            indentedImage = I2.submatrix(range - i, range - j,
                                         I1.n_rows - 2 * range, I1.n_cols - 2 * range);  
            if (i == -range && j == -range) {
                checker = meanSqError(fixedImage, indentedImage);
                shift.shift_row = i;
                shift.shift_col = j;
            } else if (((tmp = meanSqError(fixedImage, indentedImage)) < checker)) {
                checker = tmp;
                shift.shift_row = i;
                shift.shift_col = j;
            }
        }
    }

    return shift;
}

Image cutEdge(Image srcImage, int threshold1, int threshold2) {
    Image edge = canny(srcImage, threshold1, threshold2);

    uint left_edge = 0, right_edge = edge.n_cols - 1, top_edge = 0, bot_edge = edge.n_rows - 1;
    
    uint maximum = 0;

    for (uint i = 0; i < edge.n_rows * 0.1; i++) {
        uint counter = 0;
        for (uint j = 0; j < edge.n_cols; j++) {
            if (get<0>(edge(i, j)) == 255) {
                counter++;
            }
        }
        if (counter > maximum && counter > edge.n_cols / 10) {
            maximum = counter;
            top_edge = i;
        }
        
    }

    maximum = 0;
    for (uint i = edge.n_rows - 1; i >= edge.n_rows * 0.9; i--) {
        uint counter = 0;
        for (uint j = 0; j < edge.n_cols; j++) {
            if (get<0>(edge(i, j)) == 255) {
                counter++;
            }
        }
        if (counter > maximum && counter > edge.n_cols / 10) {
            maximum = counter;
            bot_edge = i;
        }
    }

    maximum = 0;
    for (uint i = 1; i < edge.n_cols * 0.1; i++) {
        uint counter = 0;
        for (uint j = 0; j < edge.n_rows; j++) {
            if (get<0>(edge(j, i)) == 255) {
                counter++;
            }
        }
        if (counter > maximum && counter > edge.n_rows / 10) {
            maximum = counter;
            left_edge = i;
        }
    }

    maximum = 0;
    for (uint i = edge.n_cols - 1; i > edge.n_cols * 0.9; i--) {
        uint counter = 0;
        for (uint j = 0; j < edge.n_rows; j++) {
            if (get<0>(edge(j, i)) == 255) {
                counter++;
            }
        }
        if (counter > maximum && counter > edge.n_rows / 10) {
            maximum = counter;
            right_edge = i;
        }
    }
    return srcImage.submatrix(top_edge, left_edge, 
                              bot_edge -  top_edge,
                              right_edge - left_edge);
}

Image align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
            bool isInterp, bool isSubpixel, double subScale)
{
    uint hight = srcImage.n_rows / 3;
    Image blue, green, red;
    Image r, g, b;
    Image rWithoutEdge, gWithoutEdge, bWithoutEdge;
    shiftCord shiftG, shiftR;    
    int rows, cols;

    b = srcImage.submatrix(0, 0, hight, srcImage.n_cols);
    g = srcImage.submatrix(hight, 0, hight, srcImage.n_cols);
    r = srcImage.submatrix(2 * hight, 0, hight, srcImage.n_cols);

    rWithoutEdge = cutEdge(r, 40, 100);
    gWithoutEdge = cutEdge(g, 40, 100);
    bWithoutEdge = cutEdge(b, 40, 100);

    rows = min(rWithoutEdge.n_rows, min(gWithoutEdge.n_rows, bWithoutEdge.n_rows));
    cols = min(rWithoutEdge.n_cols, min(gWithoutEdge.n_cols, bWithoutEdge.n_cols));

    
    red   = rWithoutEdge.submatrix(0, 0, rows, cols);
    green = gWithoutEdge.submatrix(0, 0, rows, cols);
    blue  = bWithoutEdge.submatrix(0, 0, rows, cols);

    shiftG = imageShift(blue, green, RANGE);
    shiftR = imageShift(blue, red, RANGE);
    
    Image colorizedImage(red.n_rows, red.n_cols);

    for (uint i = 0; i < colorizedImage.n_rows; i++) {
        for (uint j = 0; j < colorizedImage.n_cols; j++) {
            colorizedImage(i, j) = make_tuple(get<0>(red(idxCheck(i - shiftR.shift_row, 0, red.n_rows - 1), idxCheck(j - shiftR.shift_col, 0, red.n_cols - 1))), 
                                   get<1>(green(idxCheck(i - shiftG.shift_row, 0, red.n_rows - 1), idxCheck(j - shiftG.shift_col, 0, red.n_cols - 1))),
                                   get<2>(blue(i, j)));
        }
    }

    if (isPostprocessing) {
        if (postprocessingType == "--gray-world") {
            return gray_world(colorizedImage);
        } else if (postprocessingType == "--unsharp") {
            return unsharp(colorizedImage);
        } else if (postprocessingType == "--autocontrast") {
            return autocontrast(colorizedImage, fraction);
        }
    }
    return colorizedImage;
}

Image sobel_x(Image src_image) {
    Matrix<double> kernel = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
    return custom(src_image, kernel);
}

Image sobel_y(Image src_image) {
    Matrix<double> kernel = {{ 1,  2,  1},
                             { 0,  0,  0},
                             {-1, -2, -1}};
    return custom(src_image, kernel);
}

Image unsharp(Image src_image) {
    Matrix<double> kernel = {{ -1.0/6, -2.0/3, -1.0/6},
                             { -2.0/3, 13.0/3, -2.0/3},
                             { -1.0/6, -2.0/3, -1.0/6}};
    return custom(src_image, kernel);
}

Image gray_world(Image src_image) 
{
    uint r_sum = 0, g_sum = 0, b_sum = 0;
    double r_avg, g_avg, b_avg;
    double s_avg;
    uint pixelNum = src_image.n_rows * src_image.n_cols;
    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            r_sum += get<0>(src_image(i, j));
            g_sum += get<1>(src_image(i, j));
            b_sum += get<2>(src_image(i, j));
        }
    }

    r_avg = r_sum / pixelNum;
    g_avg = g_sum / pixelNum;
    b_avg = b_sum / pixelNum;
    s_avg = (r_avg + g_avg + b_avg ) / 3;

    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            src_image(i, j) = make_tuple(min(get<0>(src_image(i, j)) * s_avg / r_avg, 255.0),
                                         min(get<1>(src_image(i, j)) * s_avg / g_avg, 255.0),
                                         min(get<2>(src_image(i, j)) * s_avg / b_avg, 255.0));
        }
    }

    return src_image;
}

Image resize(Image src_image, double scale) {
    return src_image;
}

Image mirror(Image src_image, int hight, int width) {

    Image ans(src_image.n_rows + 2 * hight, src_image.n_cols + 2 * width);
    int i_start, i_end, j_start, j_end;
    i_start = -hight;
    i_end   = src_image.n_rows;
    j_start = -width;
    j_end   = src_image.n_cols;

    for (int i = i_start; i < i_end; i++) {
        for (int j = j_start; j < j_end; j++) {
            ans(i + hight, j + width) = src_image(abs(i), abs(j));
        }
    }

    i_start = 0;
    i_end   = src_image.n_rows + hight;
    j_start = 0;
    j_end   = width;

    for (int i = i_start; i < i_end; i++) {
        for (int j = j_start; j <= j_end; j++) {
            ans(i, j + src_image.n_cols + width - 1) = 
                ans(i, -j + src_image.n_cols + width - 1);
        }
    }

    i_start = 0;
    i_end   = hight;
    j_start = 0;
    j_end   = ans.n_cols;

    for (int i = i_start; i <= i_end; i++) {
        for (int j = j_start; j < j_end; j++) {
            ans(i + src_image.n_rows + hight - 1, j) = ans(src_image.n_rows + hight - i - 1, j);
        }
    }
    return ans;
}

Image custom(Image src_image, Matrix<double> kernel) {

    int hight = (kernel.n_rows - 1) / 2;
    int width = (kernel.n_cols - 1) / 2;

    Image mirrorIm = mirror(src_image, hight, width);
    Image ans(src_image.n_rows, src_image.n_cols);

    for (uint i = hight; i < mirrorIm.n_rows - hight; i++) {
        for (uint j = width; j < mirrorIm.n_cols - width; j++) {

            double valueR = 0, valueG = 0, valueB = 0;
            for(uint x = 0; x < kernel.n_rows; x++) {
                for (uint y = 0; y < kernel.n_cols; y++) {
                    valueR += kernel(x, y) * get<0>(mirrorIm(i + x - hight, j + y - width));
                    valueG += kernel(x, y) * get<1>(mirrorIm(i + x - hight, j + y - width));
                    valueB += kernel(x, y) * get<2>(mirrorIm(i + x - hight, j + y - width));
                }
            }
            ans(i - hight, j - width) = make_tuple(idxCheck(valueR, 0, 255), 
                                                   idxCheck(valueG, 0, 255), 
                                                   idxCheck(valueB, 0, 255));
        }   
    }

    return ans;
}

Image autocontrast(Image src_image, double fraction) {
    uint Y_arr[256] = {0};
    int Y_max = 255, Y_min = 0;
    uint hyst_start = fraction * src_image.n_cols * src_image.n_rows;
    uint hyst_end   = (1 - fraction) * src_image.n_cols * src_image.n_rows;
    uint start = 0;
    uint end = src_image.n_cols * src_image.n_rows;
    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            uint index = 0.2125 * get<0>(src_image(i, j)) + 0.7154 * get<1>(src_image(i, j)) + 0.0721 * get<2>(src_image(i, j));
            Y_arr[index]++;
        }
    }
    
    for (int i = 0; i < 256; i++) {
        start += Y_arr[i];
        if (start >= hyst_start) {
            Y_min = i;
            break;
        }
    }

    for (int i = 255; i >= 0; i--) {
        end -= Y_arr[i];
        if (end <= hyst_end) {
            Y_max = i;
            break;
        }
    }
    
    double k = 255.0 / (Y_max - Y_min);
    
    for (uint i = 0; i < src_image.n_rows; i++) {
        for (uint j = 0; j < src_image.n_cols; j++) {
            uint r = idxCheck((static_cast<int>(get<0>(src_image(i, j))) - Y_min) * k, 0, 255);
            uint g = idxCheck((static_cast<int>(get<1>(src_image(i, j))) - Y_min) * k, 0, 255);
            uint b = idxCheck((static_cast<int>(get<2>(src_image(i, j))) - Y_min) * k, 0, 255);
            src_image(i, j) = make_tuple(r, g, b);            
        }
    }

    return src_image;
}


Image gaussian(Image src_image, double sigma, int radius)
{
    double s = 2.0 * sigma * sigma;
    Matrix<double>kernel(2 * radius + 1, 2 * radius + 1);
    double sum = 0.0;

    for (int i = -radius; i <= radius; i++) {
        for(int j = -radius; j <= radius; j++) {
            kernel(i + radius, j + radius) = (exp(-(i*i + j*j)/s))/(M_PI * s);
            sum += kernel(i + radius, j + radius);
        }
    }
    
    for(int i = 0; i <= 2 * radius; i++) {
        for(int j = 0; j <= 2 * radius; j++) {
            kernel(i, j) /= sum;
        }
    }

    Image ans = custom(src_image, kernel);

    return ans;
}
 
Image gaussian_separable(Image src_image, double sigma, int radius) {

    double s = 2.0 * sigma * sigma;
    Matrix<double>kernel_X(1, 2 * radius + 1);
    Matrix<double>kernel_Y(2 * radius + 1, 1);

    double sum = 0.0;

    for (int i = -radius; i <= radius; i++) {
        kernel_X(0, i + radius) = (exp(-(i*i)/s))/(M_PI * s);
        sum += kernel_X(0, i + radius);
    }

    for(int i = 0; i <= 2 * radius; i++) {
        kernel_X(0, i) /= sum;
        kernel_Y(i, 0) = kernel_X(0, i);
    }

    Image ans = custom(custom(src_image, kernel_Y), kernel_X);

    return ans;
}

Image median(Image src_image, int radius) {
    Image ans(src_image.n_rows - 2 * radius, src_image.n_cols - 2 * radius);
    int kernelSize = 2 * radius + 1;
    int kernelVol = kernelSize * kernelSize;
    uint *arrR = new uint[kernelVol];
    uint *arrG = new uint[kernelVol];
    uint *arrB = new uint[kernelVol];

    for (uint i = radius; i < src_image.n_rows - radius; i++) {
        for (uint j = radius; j < src_image.n_cols - radius; j++) {

            for(int x = -radius; x <= radius; x++) {
                for (int y = -radius; y <= radius; y++) {
                    arrR[(x + radius) * kernelSize + (y + radius)] = get<0>(src_image(i + x, j + y));
                    arrG[(x + radius) * kernelSize + (y + radius)] = get<1>(src_image(i + x, j + y));
                    arrB[(x + radius) * kernelSize + (y + radius)] = get<2>(src_image(i + x, j + y));
                }
            }
            sort(arrR, arrR + kernelVol);
            sort(arrG, arrG + kernelVol);
            sort(arrB, arrB + kernelVol);
            ans(i - radius, j - radius) = make_tuple(arrR[kernelVol/2], 
                                                     arrG[kernelVol/2], 
                                                     arrB[kernelVol/2]);
        }
    }
    delete []arrR;
    delete []arrG;
    delete []arrB;
    return ans;
}

Image median_linear(Image src_image, int radius) {
    return src_image;
}

Image median_const(Image src_image, int radius) {
    return src_image;
}

shiftCord tetaNeighbor(double teta) {
    shiftCord cur_shift;

    if (teta > M_PI / 4 && teta < 3 * M_PI / 4) {
        cur_shift.shift_row = 1;
    } else  if (teta > -3 * M_PI / 4 && teta < -M_PI / 4) {
        cur_shift.shift_row = -1;
    }

    if (teta > -M_PI / 4 && teta < M_PI / 4) {
        cur_shift.shift_col = 1;
    } else  if (teta < -3 * M_PI / 4 && teta > 3 * M_PI / 4) {
        cur_shift.shift_col = -1;
    }
    
    return cur_shift;
}


Image canny(Image src_image, int threshold1, int threshold2) {
    Image filteredImage = gaussian(src_image, 1.4, 2);

    Image I_x = sobel_x(filteredImage); 
    Image I_y = sobel_y(filteredImage);

    Matrix<double> G(I_x.n_rows, I_x.n_cols);
    Matrix<double> teta(I_x.n_rows, I_x.n_cols);

    for (uint i = 0; i < I_x.n_rows; i++) {
        for (uint j = 0; j < I_x.n_cols; j++) {
            G(i, j) = sqrt(get<0>(I_x(i, j)) * get<0>(I_x(i, j)) + get<0>(I_y(i, j)) * get<0>(I_y(i, j)));
            teta(i, j) = atan2(get<0>(I_y(i, j)) , get<0>(I_x(i, j)));
        }
    }
    
    Image ans(I_x.n_rows, I_y.n_cols);
    Matrix<bool> hysteresis;
    for (uint i = 1; i < I_x.n_rows - 1; i++) {
        for (uint j = 1; j < I_x.n_cols - 1; j++) {
            shiftCord neighbor;
            neighbor = tetaNeighbor(teta(i, j));
            if (G(i, j) <= G(i + neighbor.shift_row, j + neighbor.shift_col) ||
                G(i, j) <= G(i - neighbor.shift_row, j - neighbor.shift_col) ||
                G(i, j) < threshold1) {
                ans(i, j) = make_tuple(0, 0, 0);
            } else if (G(i, j) > threshold2) {
                ans(i, j) = make_tuple(255, 255, 255);
            }
        }
    }
    for (uint i = 1; i < ans.n_rows - 1; i++) {
        for (uint j = 1; j < ans.n_cols - 1; j++) {
            if (get<0>(ans(i, j)) == 255) {
                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        if (get<0>(ans(i + k, j + l)) != 0) {
                            ans(i, j) = make_tuple(255, 255, 255);
                        }
                    }
                }
            }
        }
    }
    return ans;
}
