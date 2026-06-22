#include <vector>
#include <algorithm>
#include <boost/compute.hpp>
namespace compute = boost::compute;

int main() {
    // 1. 获取默认 GPU 设备
    compute::device gpu = compute::system::default_device();
    
    // 2. 创建上下文和命令队列
    compute::context ctx(gpu);
    compute::command_queue queue(ctx, gpu);
    
    // 3. 在主机上生成随机数
    std::vector<float> host_vector(1000000);
    std::generate(host_vector.begin(), host_vector.end(), rand);
    
    // 4. 在设备上创建向量并拷贝数据
    compute::vector<float> device_vector(1000000, ctx);
    compute::copy(host_vector.begin(), host_vector.end(), device_vector.begin(), queue);
    
    // 5. ⭐ 在 GPU 上排序
    compute::sort(device_vector.begin(), device_vector.end(), queue);
    
    // 6. 拷贝结果回主机
    compute::copy(device_vector.begin(), device_vector.end(), host_vector.begin(), queue);
    return 0;
}
