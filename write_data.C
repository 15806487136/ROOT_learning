void write_data() {
    // 1. 创建一个新文件 "experiment_data.root"
    // "RECREATE" 意思是：如果文件已存在，就覆盖它
    TFile *file = new TFile("experiment_data.root", "RECREATE");

    // 2. 创建一个 "表格" (TTree)
    // 名字叫 "tree"，标题叫 "Simulated Data"
    TTree *tree = new TTree("tree", "Simulated Data");

    // 3. 定义变量来暂存数据
    double momentum; // 动量
    double energy;   // 能量

    // 4. 创建 "列" (Branch)
    // 意思是：在 tree 里建一列叫 "px"，它的数据来源是内存里的 &momentum 变量
    // "px/D" 意思是这一列存的是 Double 类型
    tree->Branch("px", &momentum, "px/D");
    tree->Branch("E", &energy, "E/D");

    // 5. 模拟 10000 个事件
    for (int i = 0; i < 10000; i++) {
        // 生成模拟数据 (高斯分布)
        momentum = gRandom->Gaus(10, 2); // 均值10，宽度2
        energy = momentum * momentum + 0.5; // 假设简单的能量公式

        // 关键一步：把当前 momentum 和 energy 的值填入表格的一行
        tree->Fill(); 
    }

    // 6. 保存并关闭
    file->Write(); // 把内存里的树写入硬盘
    file->Close();
    
    cout << "数据已生成: experiment_data.root" << endl;
}
