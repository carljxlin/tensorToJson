#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <type_traits>

using namespace Eigen;
using namespace rapidjson;
using namespace std;

/*
假设输入如下
 Tensor<int, 2> tensor([[1,2], [3,4]]), doc, [2,2], idx = 0
目标json为
{
  "0": [1, 2],
  "1": [3, 4]
}
执行流程如下：
chip(0,0) get Tensor<int, 1> = [1,2], 递归调用TensorToJson，返回结果subDoc贴入json
chip(1,0) get Tensor<int, 1> = [1,2], 递归调用TensorToJson，返回结果subDoc贴入json
调用 TensorToJson(Tensor<int, 1> tensor([1,2]), doc, [2,2], idx = 1)
 * */
// 处理一维张量的情况
template<typename T>
void TensorToJson(const Tensor<T, 1>& tensor, Document& doc, const vector<int>& dimensions, int idx = 0) {
    Value json_array(kArrayType);
    for (int i = 0; i < dimensions[idx]; ++i) {
        json_array.PushBack(Value(tensor(i)).Move(), doc.GetAllocator());
    }
    doc.CopyFrom(json_array, doc.GetAllocator());
}

// 处理多维张量的情况
template<typename T, int DIM>
void TensorToJson(const Tensor<T, DIM>& tensor, Document& doc, const vector<int>& dimensions, int idx = 0) {
    if (idx < dimensions.size() - 1) {
        Value json_array(kArrayType);
        for (int i = 0; i < dimensions[idx]; ++i) {
            Document subDoc;
            subDoc.SetArray();
            auto& allocator = subDoc.GetAllocator();
            auto subTensor = tensor.chip(i, 0).eval();
            TensorToJson(subTensor, subDoc, dimensions, idx + 1);
            json_array.PushBack(subDoc, doc.GetAllocator());
        }
        doc.CopyFrom(json_array, doc.GetAllocator());
    }
}


/*
template<typename T, int DIM>
void TensorToJson(const Tensor<T, DIM>& tensor, Document& doc, const vector<int>& dimensions, int idx = 0) {
    if (idx >= dimensions.size() - 1) {
        Tensor<T, 1> tensor1D = tensor.eval();
        Value json_array(kArrayType);
        for (int i = 0; i < tensor.dimension(0); ++i) {
            json_array.PushBack(Value(tensor1D(i)).Move(), doc.GetAllocator());
        }
        doc.CopyFrom(json_array, doc.GetAllocator());
    } else {
        Value json_array(kArrayType);
        for (int i = 0; i < tensor.dimension(idx); ++i) {
            auto subTensor = tensor.chip(i, 0).eval(); // rank - 1
            Document subDoc(&doc.GetAllocator());
            TensorToJson(subTensor, subDoc, dimensions, idx + 1);
            json_array.PushBack(subDoc, doc.GetAllocator());
        }
        doc.CopyFrom(json_array, doc.GetAllocator());
    }
}
 */

template<typename T>
Tensor<T, Dynamic> ReadTensorFromFile(ifstream& file, const vector<int>& dimensions) {
    // 计算元素总数
    int totalSize = 1;
    for (int dim : dimensions) {
        totalSize *= dim;
    }

    // 初始化Tensor
    Tensor<T, Dynamic> tensor(totalSize);
    tensor.resize(dimensions);

    // 读取数据
    T val;
    for (int i = 0; i < totalSize; ++i) {
        file >> val;
        tensor.data()[i] = val;
    }

    return tensor;
}

int main() {
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
            Tensor<int, Dynamic> tensor = ReadTensorFromFile<int>(inFile, dimensions);
            TensorToJson(tensor, doc, dimensions);
        } else if (type == "float") {
            Tensor<float, Dynamic> tensor = ReadTensorFromFile<float>(inFile, dimensions);
            TensorToJson(tensor, doc, dimensions);
        } else if (type == "char") {
            Tensor<char, Dynamic> tensor = ReadTensorFromFile<char>(inFile, dimensions);
            TensorToJson(tensor, doc, dimensions);
        }

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);

        ofstream outFile("case" + to_string(test_case++) + ".json");
        outFile << buffer.GetString();
        outFile.close();
    }

    inFile.close();
    return 0;
}