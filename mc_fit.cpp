#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "TAxis.h"
#include "ROOT/RDataFrame.hxx"

// 使用 RooFit 命名空间，这样就不用每次都写 RooFit::Save() 等
using namespace RooFit;

void mc_fit() {
    // 1. 基础配置：开启多线程并行加速
    ROOT::EnableImplicitMT();

    // 2. 数据准备
    // ---------------------------------------------------------

    // 定义观察量
    RooRealVar m_yy("m_yy", "m_{#gamma#gamma} [GeV]", 105.0, 160.0);
    RooRealVar weight("weight", "event weight", -1e6, 1e6);
    
    // 按照作业要求设置 30 bins (用于画图)
    m_yy.setBins(30);

    // 加载 ROOT 文件
    TFile *tmp_file = TFile::Open("temp_mc_processed.root");
    if (!tmp_file || tmp_file->IsZombie()) {
        std::cerr << "Error: Cannot open file temp_mc_processed.root" << std::endl;
        return;
    }

    TTree *tmp_tree = (TTree*)tmp_file->Get("small_tree");
    if (!tmp_tree) {
        std::cerr << "Error: Cannot find tree 'small_tree'" << std::endl;
        return;
    }

    // 创建加权数据集
    // 注意：Import 需要解引用 tree (*tmp_tree)
    RooDataSet ds_mc("ds_mc", "Weighted MC ggH", 
                     RooArgSet(m_yy, weight), 
                     Import(*tmp_tree), 
                     WeightVar("weight"));

    // 3. 定义模型
    // ---------------------------------------------------------
    
    // 定义高斯 PDF
    RooRealVar mean("mean", "Gaussian Mean", 125, 120, 130);
    RooRealVar sigma("sigma", "Gaussian Sigma", 2.0, 0.5, 5.0);
    RooGaussian gauss_pdf("gauss_pdf", "Signal Gaussian", m_yy, mean, sigma);

    // 4. 执行加权拟合
    // ---------------------------------------------------------
    // SumW2Error(true) 对应 Python 的 SumW2Error(True)
    // Save() 对应 Python 的 Save()
    RooFitResult* fit_result = gauss_pdf.fitTo(ds_mc, Save(), SumW2Error(true));

    // 5. 绘图与结果输出
    // ---------------------------------------------------------
    TCanvas *canvas = new TCanvas("rf_canvas", "MC Signal Fit", 800, 600);
    
    // 创建画图框架
    RooPlot* plot = m_yy.frame(Title("Invariant Mass of Photons (Weighted MC)"));

    // 绘制数据点（显示加权后的误差棒 SumW2）
    ds_mc.plotOn(plot, DataError(RooAbsData::SumW2));
    
    // 绘制拟合曲线
    gauss_pdf.plotOn(plot, LineColor(kRed));
    
    // 在图上显示拟合参数框
    gauss_pdf.paramOn(plot, Layout(0.65, 0.9, 0.9));

    plot->Draw();
    canvas->SaveAs("mc_fit_cpp.png");

    // 6. 打印结果
    // ---------------------------------------------------------
    std::cout << "\n" << std::string(40, '=') << std::endl;
    std::cout << "拟合收敛状态: " << fit_result->status() << std::endl;
    std::cout << "提取出的信号参数 (用于后续 Data 拟合):" << std::endl;
    
    // 设置输出精度
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(4);

    std::cout << "  FINAL MEAN:  " << mean.getVal() << " +/- " << mean.getError() << " GeV" << std::endl;
    std::cout << "  FINAL SIGMA: " << sigma.getVal() << " +/- " << sigma.getError() << " GeV" << std::endl;
    std::cout << std::string(40, '=') << std::endl;

    // 清理内存 (可选，ROOT 通常会自动处理)
    // tmp_file->Close(); 
}