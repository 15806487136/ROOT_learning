import ROOT
import os

def main():
    # 1. 基础配置
    ROOT.EnableImplicitMT() 
    # 按照作业要求：画图范围 105-160，30个bin
    n_bins = 30  
    m_min, m_max = 105.0, 160.0
    
    # -------------------------------------------------------
    # 2. 直接从加工后的数据文件读取
    # -------------------------------------------------------
    m_yy = ROOT.RooRealVar("m_yy", "m_{#gamma#gamma} [GeV]", m_min, m_max)
    m_yy.setBins(n_bins)

    tf = ROOT.TFile.Open("temp_data_for_fit.root")
    tree = tf.Get("small_tree")
    ds_data = ROOT.RooDataSet("ds_data", "Real Data", ROOT.RooArgSet(m_yy), ROOT.RooFit.Import(tree))
    print(f">>> Data事件数: {ds_data.numEntries()}")

    # -------------------------------------------------------
    # 3. RooFit 建模
    # -------------------------------------------------------

    # --- A. 信号模型 (关键修改) ---
    
    # [修正1] 直接填入你MC算出的 Mean (124.8970)，且不要给范围！
    # [修正2] 必须设置为 Constant (固定)，禁止拟合器修改它
    mean = ROOT.RooRealVar("mean", "Signal Mean", 124.8970)
    mean.setConstant(True) 
    
    # [修正3] 直接填入你MC算出的 Sigma (2.9402)，同样固定
    sigma = ROOT.RooRealVar("sigma", "Signal Sigma", 2.9402)
    sigma.setConstant(True)

    sig_pdf = ROOT.RooGaussian("sig_pdf", "Signal Gaussian", m_yy, mean, sigma)

    # --- B. 本底模型 (3阶多项式) ---
    # 作业要求 3rd order polynomial。
    # 使用 RooChebychev 拟合通常比 Bernstein 更稳定，不容易出现奇怪的波浪
    # 3阶多项式需要 3 个参数 (a1, a2, a3)
    a1 = ROOT.RooRealVar("a1", "a1", -0.5, -2, 2)
    a2 = ROOT.RooRealVar("a2", "a2", 0.1, -2, 2)
    a3 = ROOT.RooRealVar("a3", "a3", 0.01, -2, 2)
    
    # 如果老师强制要求用 Bernstein，也可以换回 RooBernstein，但 Chebychev 收敛性更好
    bkg_pdf = ROOT.RooChebychev("bkg_pdf", "Background", m_yy, ROOT.RooArgList(a1, a2, a3))

    # --- C. 总模型 ---
    # 信号产额 nsig：初始值给小一点，比如 50 或 100，范围允许负值以保证无偏估计
    nsig = ROOT.RooRealVar("nsig", "Signal Yield", 50, -500, 2000)
    
    # 本底产额 nbkg：初始值给总事件数
    nbkg = ROOT.RooRealVar("nbkg", "Background Yield", ds_data.numEntries(), 0, ds_data.numEntries()*1.5)

    total_model = ROOT.RooAddPdf("total_model", "Signal + Background", 
                                 ROOT.RooArgList(sig_pdf, bkg_pdf), 
                                 ROOT.RooArgList(nsig, nbkg))

    # -------------------------------------------------------
    # 4. 拟合与画图
    # -------------------------------------------------------
    print(">>> 开始 Data 拟合...")
    
    # [修正4] 使用 Extended(True) 拟合，这样才能正确算出 Yield 的误差
    # Save() 保存结果，PrintLevel(-1) 减少刷屏
    fit_res = total_model.fitTo(ds_data, ROOT.RooFit.Save(), ROOT.RooFit.Extended(True))
    
    # 画图
    c = ROOT.TCanvas("c", "Data Fit", 800, 600)
    plot = m_yy.frame(ROOT.RooFit.Title("H->yy Data Fit (Mean Fixed)"))

    ds_data.plotOn(plot, ROOT.RooFit.Name("data"))
    
    # 画全模型 (红色)
    total_model.plotOn(plot, ROOT.RooFit.Name("total_fit"), ROOT.RooFit.LineColor(ROOT.kRed))
    
    # 画本底分量 (蓝色虚线)
    total_model.plotOn(plot, ROOT.RooFit.Components("bkg_pdf"), 
                       ROOT.RooFit.Name("bkg_fit"), 
                       ROOT.RooFit.LineStyle(ROOT.kDashed), 
                       ROOT.RooFit.LineColor(ROOT.kBlue))

    # 显示参数框
    total_model.paramOn(plot, ROOT.RooFit.Layout(0.60, 0.90, 0.85))

    plot.Draw()
    
    c.SaveAs("data_fit.png")
    
    print("\n" + "="*40)
    print(f"拟合状态: {fit_res.status()} (0=成功)")
    print(f"Mean (Fixed)  : {mean.getVal():.4f} GeV")
    print(f"Sigma (Fixed) : {sigma.getVal():.4f} GeV")
    print(f"Signal Yield  : {nsig.getVal():.2f} +/- {nsig.getError():.2f}")
    
    # 简单的显著性估算 S/sqrt(S+B) 或者 S/error
    significance = nsig.getVal() / nsig.getError() if nsig.getError() > 0 else 0
    print(f"Significance  : {significance:.2f} sigma (approx)")
    print("="*40)

if __name__ == "__main__":
    main()
