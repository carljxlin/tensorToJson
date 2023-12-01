#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <iostream>
#include <array>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>
using namespace Eigen;
using namespace std;
using namespace rapidjson;

/*
void test1() {
    // 声明一个具有动态维度的 DSizes 对象
    Eigen::DSizes<ptrdiff_t, Eigen::Dynamic> dimensions(3); // 3 表示维度数量

    // 初始化维度值
    dimensions[0] = 4; // 第一维大小为 4
    dimensions[1] = 5; // 第二维大小为 5
    dimensions[2] = 6; // 第三维大小为 6

    // 使用 DSizes 创建动态维度的 Tensor
    Eigen::Tensor<double, Eigen::Dynamic> tensor(dimensions);
    tensor.setRandom();

    // 打印 Tensor 信息
    std::cout << "Tensor dimensions: " << tensor.dimension(0) << "x" << tensor.dimension(1) << "x" << tensor.dimension(2) << std::endl;

    return 0;
}
*/
void test2() {
    Eigen::Tensor<double, 3> tensor(4, 5, 6); // 创建一个 4x5x6 的 Tensor
    tensor.setRandom(); // 使用随机数填充 Tensor
    Eigen::array<Eigen::DenseIndex, 3> size1 = {4, 5, 6};   // 切片的大小
    // 遍历子张量并打印其元素
    for (int i = 0; i < size1[0]; ++i) {
        for (int j = 0; j < size1[1]; ++j) {
            for (int k = 0; k < size1[2]; ++k) {
                std::cout << tensor(i, j, k) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "---" << std::endl;
    }
    std::cout<<"************"<<std::endl;
    Eigen::array<Eigen::DenseIndex, 3> offset = {1, 1, 1}; // 切片的起始索引
    Eigen::array<Eigen::DenseIndex, 3> size = {2, 3, 4};   // 切片的大小
    Eigen::Tensor<double, 3> subTensor = tensor.slice(offset, size);
    // 遍历子张量并打印其元素
    for (int i = 0; i < size[0]; ++i) {
        for (int j = 0; j < size[1]; ++j) {
            for (int k = 0; k < size[2]; ++k) {
                std::cout << subTensor(i, j, k) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "---" << std::endl;
    }
}
/*
template <typename T, int DIM>
void PrintSubTensor(const Tensor<T, DIM>& tensor, const vector<int>& offset, int currentDim = 0) {
    if (currentDim >= offset.size()) {
        // 达到 offset 指定的深度，打印子张量
        cout << tensor << endl;
        return;
    }

    // 根据当前维度和 offset 提取子张量
    auto subTensor = tensor.chip(offset[currentDim], currentDim).eval();

    // 递归调用以处理下一个维度
    PrintSubTensor(subTensor, offset, currentDim + 1);
}*/
template <typename T, int DIM>
void PrintTensor(const Tensor<T, DIM>& tensor, const vector<int>& offset) {
    if (offset.empty()) {
        // 如果 offset 为空，打印整个张量
        cout << tensor << endl;
        return;
    }

    // 根据 offset 的大小提取相应的子张量
    switch (offset.size()) {
        case 1: {
            auto subTensor = tensor.chip(offset[0], 0);
            cout << subTensor << endl;
            break;
        }
        case 2: {
            auto subTensor = tensor.chip(offset[0], 0).chip(offset[1], 0);
            cout << subTensor << endl;
            break;
        }
        case 3: {
            Tensor<T, 1> subTensor = tensor.chip(offset[0], 0).chip(offset[1], 0);
            cout << subTensor(offset[2]) << endl; // 单个元素
            break;
        }
            // 可以根据需要继续添加更多的 case
        default:
            cout << "Offset size is too large or not supported." << endl;
            break;
    }
    cout << endl;
}
/*
template <typename T, int DIM>
void PrintTensor(const Tensor<T, DIM>& tensor, const vector<int>& offset) {
    PrintSubTensor(tensor, offset);
}
*/
void test3() {
    Tensor<double, 3> tensor(2, 2, 2);
    tensor.setValues({{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}});
    //tensor.setRandom();

    PrintTensor(tensor, {1});    // 打印 [[5,6],[7,8]]
    PrintTensor(tensor, {1, 0}); // 打印 [5,6]
    PrintTensor(tensor, {1, 0, 0}); // 打印 5

}
/*
template<typename T>
auto ReadTensorFromFile(ifstream& file, const vector<int>& dimensions) {
    // 计算元素总数
    int totalSize = 1;
    for (int dim : dimensions) {
        totalSize *= dim;
    }

    // 初始化Tensor
    Eigen::Tensor<T, Eigen::Dynamic> tensor;
    ttensor.resize(Eigen::array<Eigen::Index, 1>{static_cast<Eigen::Index>(dimensions.size())});

    // 读取数据
    T val;
    for (int i = 0; i < totalSize; ++i) {
        file >> val;
        tensor.data()[i] = val;
    }
    cout<<tensor<<endl;
    return;
}*/
/*
template<typename T>
auto ReadTensorFromFile(ifstream& file, const vector<int>& dimensions) {
    // 计算元素总数
    int totalSize = 1;
    for (int dim : dimensions) {
        totalSize *= dim;
    }

    // 初始化Tensor
    Eigen::Tensor<T, Eigen::Dynamic> tensor;

    // 构建正确的多维尺寸数组
    Eigen::array<Eigen::Index, Eigen::Dynamic> tensorDimensions(dimensions.size());
    for (size_t i = 0; i < dimensions.size(); ++i) {
        tensorDimensions[i] = dimensions[i];
    }

    // 使用多维尺寸数组调整张量大小
    tensor.resize(tensorDimensions);

    // 读取数据
    T val;
    for (int i = 0; i < totalSize; ++i) {
        file >> val;
        tensor.data()[i] = val;
    }

    // 此处需要一个适当的方式来打印张量，因为直接使用 cout 可能不支持
    // 打印张量的一种方法是遍历其元素
    // ...
    cout<<tensor<<endl;
    return tensor;
}


int test4() {
    ifstream inFile("test.data");
    if (!inFile.is_open()) {
        cerr << "Unable to open file test.data" << endl;
        return 1;
    }

    string type;
    int dim, test_case = 1;
    while (inFile >> type >> dim) {
        vector<int> dimensions(dim);
        for (int i = 0; i < dim; ++i) {
            inFile >> dimensions[i];
        }

        Document doc;
        doc.SetArray();
        auto& allocator = doc.GetAllocator();

        if (type == "int") {
            ReadTensorFromFile<int>(inFile, dimensions);

        } else if (type == "float") {
            ReadTensorFromFile<float>(inFile, dimensions);

        } else if (type == "char") {
            ReadTensorFromFile<char>(inFile, dimensions);

        }

    }

    inFile.close();
    return 0;
}*/

int main() {
    test3();
}
