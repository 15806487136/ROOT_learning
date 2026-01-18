#include <iostream>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "TAxis.h"
#include "ROOT/RDataFrame.hxx"

// 引入 RooFit 命名空间，省去写 RooFit:: 的麻烦
using namespace RooFit;

void data_fit() {
    // 1. 基础配置
    ROOT::EnableImplicitMT();
    
    // 按照作业要求配置
    int n_bins = 30;
    double m_min = 105.0;
    double m_max = 160.0;

    // -------------------------------------------------------
    // 2. 读取数据
    // -------------------------------------------------------
    RooRealVar m_yy("m_yy", "m_{#gamma#gamma} [GeV]", m_min, m_max);
    m_yy.setBins(n_bins);

    // 打开文件读取 Tree
    TFile *tf = TFile::Open("temp_data_for_fit.root");
    if (!tf || tf->IsZombie()) {
        std::cerr << "Error: Cannot open temp_data_for_fit.root" << std::endl;
        return;
    }

    TTree *tree = (TTree*)tf->Get("small_tree");
    if (!tree) {
        std::cerr << "Error: Cannot find tree 'small_tree'" << std::endl;
        return;
    }

    // 导入数据到 RooDataSet
    RooDataSet ds_data("ds_data", "Real Data", RooArgSet(m_yy), Import(*tree));
    std::cout << ">>> Data事件数: " << ds_data.numEntries() << std::endl;

    // -------------------------------------------------------
    // 3. RooFit 建模
    // -------------------------------------------------------

    // --- A. 信号模型 (固定参数) ---
    // [修正1 & 2] 填入 MC 算出的 Mean 并固定
    RooRealVar mean("mean", "Signal Mean", 124.8970);
    mean.setConstant(true);

    // [修正3] 填入 MC 算出的 Sigma 并固定
    RooRealVar sigma("sigma", "Signal Sigma", 2.9402);
    sigma.setConstant(true);

    RooGaussian sig_pdf("sig_pdf", "Signal Gaussian", m_yy, mean, sigma);

    // --- B. 本底模型 (Chebychev 3阶) ---
    RooRealVar a1("a1", "a1", -0.5, -2, 2);
    RooRealVar a2("a2", "a2", 0.1, -2, 2);
    RooRealVar a3("a3", "a3", 0.01, -2, 2);

    RooChebychev bkg_pdf("bkg_pdf", "Background", m_yy, RooArgList(a1, a2, a3));

    // --- C. 总模型 ---
    // 信号 Yield: 允许为负值
    RooRealVar nsig("nsig", "Signal Yield", 50, -500, 2000);
    // 本底 Yield
    RooRealVar nbkg("nbkg", "Background Yield", ds_data.numEntries(), 0, ds_data.numEntries()*1.5);

    RooAddPdf total_model("total_model", "Signal + Background", 
                          RooArgList(sig_pdf, bkg_pdf), 
                          RooArgList(nsig, nbkg));

    // -------------------------------------------------------
    // 4. 拟合与画图
    // -------------------------------------------------------
    std::cout << ">>> 开始 Data 拟合..." << std::endl;

    // [修正4] Extended(true) 拟合
    RooFitResult* fit_res = total_model.fitTo(ds_data, Save(), Extended(true), PrintLevel(-1));

    // 画图
    TCanvas *c = new TCanvas("c", "Data Fit", 800, 600);
    RooPlot* plot = m_yy.frame(Title("H->yy Data Fit (Mean Fixed)"));

    // 画数据
    ds_data.plotOn(plot, Name("data"));

    // 画总模型 (红色)
    total_model.plotOn(plot, Name("total_fit"), LineColor(kRed));

    // 画本底分量 (蓝色虚线)
    total_model.plotOn(plot, Components("bkg_pdf"), 
                       Name("bkg_fit"), 
                       LineStyle(kDashed), 
                       LineColor(kBlue));

    // 显示参数框
    total_model.paramOn(plot, Layout(0.60, 0.90, 0.85));

    plot->Draw();
    c->SaveAs("data_fit_cpp.png");

    // -------------------------------------------------------
    // 5. 打印结果
    // -------------------------------------------------------
    std::cout << "\n" << std::string(40, '=') << std::endl;
    std::cout << "拟合状态: " << fit_res->status() << " (0=成功)" << std::endl;
    
    // 设置输出精度
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(4);
    
    std::cout << "Mean (Fixed)  : " << mean.getVal() << " GeV" << std::endl;
    std::cout << "Sigma (Fixed) : " << sigma.getVal() << " GeV" << std::endl;
    
    std::cout.precision(2);
    std::cout << "Signal Yield  : " << nsig.getVal() << " +/- " << nsig.getError() << std::endl;

    double sig_val = nsig.getVal();
    double sig_err = nsig.getError();
    double significance = (sig_err > 0) ? (sig_val / sig_err) : 0.0;

    std::cout << "Significance  : " << significance << " sigma (approx)" << std::endl;
    std::cout << std::string(40, '=') << std::endl;
}