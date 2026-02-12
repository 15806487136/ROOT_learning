void lesson3() {
    // 1. 打开文件 (老样子)
    TFile *file = new TFile("experiment_data.root", "READ");
    TTree *tree = (TTree*)file->Get("tree");

    // 2. 准备画布，这次我们把画布切成 2x2 的四块，一次看四张图
    TCanvas *c1 = new TCanvas("c1", "Data Exploration", 800, 600);
    c1->Divide(2, 2); // 切割画布

    // === 魔法开始 ===

    // 【第一张图】：直接画动量分布
    c1->cd(1); // 进入左上角的格子
    // 解释："px" 是变量名。ROOT 会自动帮你创建直方图、自动遍历、自动填充！
    tree->Draw("px"); 

    // 【第二张图】：加筛选条件 (Cut)
    c1->cd(2); // 进入右上角的格子
    // 解释：第二个参数是筛选条件。
    // "px > 12" 意思是：我只想要动量大于 12 的那些事件。
    tree->Draw("px", "px > 12"); 
    
    // 【第三张图】：画二维相关性 (2D Plot)
    c1->cd(3); // 进入左下角的格子
    // 解释："E:px" 意思是 Y轴是E，X轴是px (注意顺序是 Y:X)
    // "colz" 意思是：用颜色深浅来表示点的密集程度 (Color Z-axis)
    tree->Draw("E:px", "", "colz");

    // 【第四张图】：复杂的筛选 + 二维图
    c1->cd(4); // 进入右下角的格子
    // 意思是：画 E vs px，但只画那些 E > 100 且 px < 12 的事件
    tree->Draw("E:px", "E > 100 && px < 12", "colz");
}
