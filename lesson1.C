void lesson1() {
    // 1. 定义一个画布 (Canvas)
    // TCanvas 就像是一张白纸，我们要在上面画图
    // 参数："c1"是内部名字，"My First Plot"是窗口标题，800x600是像素大小
    TCanvas *c1 = new TCanvas("c1", "My First Plot", 800, 600);

    // 2. 定义一个直方图 (Histogram)
    // TH1F 意思是 "Histogram 1D Float" (一维浮点数直方图)
    // 参数："h_mass"是内部名字 (给ROOT看的)
    //       "Mass Distribution; Mass [GeV]; Events" 是标题 (主标题; X轴名字; Y轴名字)
    //       100 是把X轴分成100个格子 (Bins)
    //       0, 10 是X轴的范围 (从0到10)
    TH1F *hist = new TH1F("h_mass", "Mass Distribution; Mass [GeV]; Events", 100, 0, 10);

    // 3. 填充数据 (模拟实验过程)
    // 我们用一个循环，模拟 10000 个事件
    // gRandom->Gaus(5, 1) 的意思是：生成一个符合高斯分布的随机数，平均值是5，宽度(sigma)是1
    for (int i = 0; i < 10000; i++) {
        double value = gRandom->Gaus(5, 1); 
        hist->Fill(value); // 把这个数“扔”进直方图里
    }

    // 4. 美化一下 (可选)
    hist->SetFillColor(kRed); // 设置填充颜色为淡蓝色
    hist->SetLineColor(kBlack);  // 设置边框颜色为黑色

    // 5. 画出来
    // Draw() 默认只画线，加上 "HIST" 参数表示我们要画填充好的直方图样式
    hist->Draw();

    // 6. 保存图片
    c1->SaveAs("my_first_plot.png");
}
