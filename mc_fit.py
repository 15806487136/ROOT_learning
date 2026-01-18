import ROOT

# 1. 基础配置：开启多线程并行加速
ROOT.EnableImplicitMT()

# 2. RDataFrame 阶段：物理量重构与权重计算
# 作业要求：质量范围 105-160，分30个bin
# 权重计算公式：scaleFactor_PHOTON * scaleFactor_PhotonTRIGGER * scaleFactor_PILEUP * mcWeight
file_path = "mc_343981.ggH125_gamgam.GamGam.root"
df = ROOT.RDataFrame("mini", file_path)

# 定义变量与过滤条件
df_final = df.Filter("trigP == true && photon_n >= 2") \
    .Define("weight", "scaleFactor_PHOTON * scaleFactor_PhotonTRIGGER * scaleFactor_PILEUP * mcWeight") \
    .Define("p0", "ROOT::Math::PtEtaPhiEVector(photon_pt[0], photon_eta[0], photon_phi[0], photon_E[0])") \
    .Define("p1", "ROOT::Math::PtEtaPhiEVector(photon_pt[1], photon_eta[1], photon_phi[1], photon_E[1])") \
    .Define("m_yy", "(p0 + p1).M() / 1000.0") \
    .Filter("m_yy >= 105.0 && m_yy <= 160.0")

# 将处理后的轻量化数据存入临时文件（只保留质量和权重，解决内存报错）
df_final.Snapshot("small_tree", "temp_mc_processed.root", ["m_yy", "weight"])

# 3. RooFit 阶段：提取高斯拟合参数
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
canvas.SaveAs("mc_signal_gauss_fit.png")

# 打印最终提取出的参数，方便你下一步固定它们
print("\n" + "="*40)
print(f"拟合收敛状态: {fit_result.status()}")
print(f"提取出的信号参数 (用于后续 Data 拟合):")
print(f"  FINAL MEAN:  {mean.getVal():.4f} +/- {mean.getError():.4f} GeV")
print(f"  FINAL SIGMA: {sigma.getVal():.4f} +/- {sigma.getError():.4f} GeV")
print("="*40)

# 建议手动删除临时文件
# import os; os.remove("temp_mc_processed.root")