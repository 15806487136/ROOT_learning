void lesson4() {
    // 1. 准备工作：打开文件，获取 Tree
    TFile *file = new TFile("experiment_data.root", "READ");
    TTree *tree = (TTree*)file->Get("tree");

    // 2. 创建一个画布
    TCanvas *c1 = new TCanvas("c1", "Fitting Demo", 800, 600);

    // === 新知识点：把 Tree 的数据“灌”进直方图 ===
    // 以前我们用 Loop+Fill，或者直接 Draw。
    // 现在我们要把 Draw 的结果存下来，以便后面做拟合。
    // 语法："变量名 >> 直方图名字(Bin数, min, max)"
    tree->Draw("px >> h_fit(100, 0, 20)");

    // 3. 从内存里把这个刚才生成的直方图抓出来
    // "h_fit" 就是上面那个字符串里起的名字
    TH1F *hist = (TH1F*)gDirectory->Get("h_fit");

    // 4. 定义拟合函数 (The Model)
    // TF1 = TFormula 1D (一维公式)
    // 参数1："f1" 是函数的名字
    // 参数2："gaus" 是 ROOT 内置的高斯函数 (p0*exp(-0.5*((x-p1)/p2)^2))
    // 参数3,4：拟合范围 (从 5 到 15)
    TF1 *func = new TF1("f1", "gaus", 5, 15);

    // 5. 执行拟合 (Fit)
    // 意思：用 func 这个函数去逼近 hist 这个直方图
    // "R" 意思是：只拟合我们在 func 里定义的范围 (Range, 5-15)，忽略其他地方
    hist->Fit(func, "R");

    // 6. 获取结果 (Get Results)
    // gaus 函数有3个参数：
    // [0] Constant (高度)
    // [1] Mean (均值/位置)
    // [2] Sigma (宽度)
    double mean = func->GetParameter(1); // 获取第1号参数
    double error = func->GetParError(1); // 获取第1号参数的误差

    // 打印出来装个逼
    cout << "==================================" << endl;
    cout << "测量结果：" << endl;
    cout << "粒子质量 (Mean) = " << mean << " +/- " << error << " GeV" << endl;
    cout << "==================================" << endl;
    
    // 7. 美化一下：把统计框也就是右上角的那个框框稍微改改，显示拟合结果
    gStyle->SetOptFit(1111); // 这是一个魔法数字，让统计框显示概率、卡方、参数等
    hist->Draw(); // 刷新一下显示
}
