import ROOT

# 1. 基础配置：开启多线程并行加速
ROOT.EnableImplicitMT()

# 2. 直接从加工后的文件读取数据
# ---------------------------------------------------------

# 定义观察量（名称需与树中的列名完全对应）
m_yy = ROOT.RooRealVar("m_yy", "m_{#gamma#gamma} [GeV]", 105.0, 160.0)
weight = ROOT.RooRealVar("weight", "event weight", -1e6, 1e6)
m_yy.setBins(30) # 按照作业要求设置 30 bins

# 加载加权数据集
tmp_file = ROOT.TFile.Open("temp_mc_processed.root")
tmp_tree = tmp_file.Get("small_tree")
ds_mc = ROOT.RooDataSet(
    "ds_mc", "Weighted MC ggH", 
    ROOT.RooArgSet(m_yy, weight), 
    ROOT.RooFit.Import(tmp_tree), 
    ROOT.RooFit.WeightVar("weight")
)

# 定义高斯 PDF
# 均值初始设为 125 GeV，Sigma 初始设为 2.0 GeV
mean = ROOT.RooRealVar("mean", "Gaussian Mean", 125, 120, 130)
sigma = ROOT.RooRealVar("sigma", "Gaussian Sigma", 2.0, 0.5, 5.0)
gauss_pdf = ROOT.RooGaussian("gauss_pdf", "Signal Gaussian", m_yy, mean, sigma)

# 4. 执行加权拟合
# SumW2Error(True) 确保对加权后的统计误差进行正确处理
fit_result = gauss_pdf.fitTo(ds_mc, ROOT.RooFit.Save(), ROOT.RooFit.SumW2Error(True))

# 5. 绘图与结果输出
canvas = ROOT.TCanvas("rf_canvas", "MC Signal Fit", 800, 600)
plot = m_yy.frame(ROOT.RooFit.Title("Invariant Mass of Photons (Weighted MC)"))

# 绘制数据点（显示加权后的误差棒）
ds_mc.plotOn(plot, ROOT.RooFit.DataError(ROOT.RooAbsData.SumW2))
# 绘制拟合曲线
gauss_pdf.plotOn(plot, ROOT.RooFit.LineColor(ROOT.kRed))
# 在图上显示拟合参数框
gauss_pdf.paramOn(plot, ROOT.RooFit.Layout(0.65, 0.9, 0.9))

plot.Draw()
canvas.SaveAs("mc_fit.png")

# 打印最终提取出的参数，方便你下一步固定它们
print("\n" + "="*40)
print(f"拟合收敛状态: {fit_result.status()}")
print(f"提取出的信号参数 (用于后续 Data 拟合):")
print(f"  FINAL MEAN:  {mean.getVal():.4f} +/- {mean.getError():.4f} GeV")
print(f"  FINAL SIGMA: {sigma.getVal():.4f} +/- {sigma.getError():.4f} GeV")
print("="*40)

# 建议手动删除临时文件
# import os; os.remove("temp_mc_processed.root")
