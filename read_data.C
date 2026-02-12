void read_data() {
    // 1. 打开刚才生成的文件
    // "READ" 表示只读模式，很安全
    TFile *file = new TFile("experiment_data.root", "READ");

    // 2. 获取那个表格 (TTree)
    // 注意：我们要用 file->Get() 来从文件里把树 "抓" 出来
    TTree *tree = (TTree*)file->Get("tree");

    // 3. 准备钩子 (Hook)
    // 我们需要定义一个变量，用来 "接住" 读取出来的数据
    double value_px; 
    
    // 4. 连接钩子 (SetBranchAddress)
    // 意思是：当读取 "px" 这一列时，把数据放进 value_px 这个变量的内存地址(&)里
    tree->SetBranchAddress("px", &value_px);

    // 5. 准备直方图
    TH1F *hist = new TH1F("h_px", "Momentum from File; Px [GeV]; Events", 100, 0, 20);

    // 6. 循环读取每一行 (Event Loop)
    // GetEntries() 会告诉我们需要跑多少次循环
    int nEntries = tree->GetEntries();
    
    for (int i = 0; i < nEntries; i++) {
        // 关键动作：读取第 i 行数据
        // 这句话执行完，value_px 里就是第 i 个事件的动量了
        tree->GetEntry(i); 

        // 此时 value_px 已经是我们需要的值了，把它填进直方图
        hist->Fill(value_px);
    }

    // 7. 画图
    TCanvas *c1 = new TCanvas();
    hist->Draw();
}
