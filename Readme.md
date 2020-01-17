# 一个C++实行的协程demo

支持Risc-V和x64上的Windows和Linux。
## 编译指南
```bash
# 克隆仓库
git clone <url>
cd coroutine
# 编译
mkdir build && cd build
cmake .. && make
# 测试
make test
# 运行
sudo ./coroutine
```
Windows请用msvc编译，Linux请用gcc编译
