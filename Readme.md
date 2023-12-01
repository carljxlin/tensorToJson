运行测试程序

make test
./test


运行tensorToJson

make tensorToJson

./tensorToJson

固定层数的tensor转json已经实现了，有一些当时运行后的结果，比如case1.json。但是n维的tensor转json目前还难以实现。主要困难点在于使用Eigen::Dynamic后编译无法通过。晚点我再查一下是什么原因导致的，修复一下。
