import numpy
from ROOT import TH1F, TH2F, TCanvas, TFile, TNtuple, TProfile, gBenchmark, gSystem
 

 
# Create some histograms, a profile histogram and an ntuple
hpx = TH1F("hpx", "This is the px distribution", 100, -4, 4)
hpxpy = TH2F("hpxpy", "py vs px", 40, -4, 4, 40, -4, 4)
hprof = TProfile("hprof", "Profile of pz versus px", 100, -4, 4, 0, 20)
ntuple = TNtuple("ntuple", "Demo ntuple", "px:py:pz:random:i")
 
# Set canvas/frame attributes.
hpx.SetFillColor(48)
 
gBenchmark.Start("hsimple")
 
# Fill histograms randomly.
for i in range(25000):
    # Retrieve the generated values
    px, py = numpy.random.standard_normal(size=2)
 
    pz = px * px + py * py
    random = numpy.random.rand()
 
    # Fill histograms.
    hpx.Fill(px)
    hpxpy.Fill(px, py)
    hprof.Fill(px, pz)
    ntuple.Fill(px, py, pz, random, i)
 

 
gBenchmark.Show("hsimple")
 
# 创建一个新的画布并分割成两行
c2 = TCanvas("c2", "hpx and hpxpy", 800, 600)
c2.Divide(1, 2)  # 1列，2行

# 第一行：绘制 hpx
c2.cd(1)  # 切换到第一面板
hpx.Draw()

# 第二行：绘制 hpxpy
c2.cd(2)  # 切换到第二面板
hpxpy.Draw("COLZ")  # 使用 COLZ 选项显示颜色

c2.Modified()
c2.Update()

# 保存画布为 PNG 和 PDF
c2.SaveAs("c2_plot.png")
c2.SaveAs("c2_plot.pdf")

# Create a new ROOT binary machine independent file.
# Note that this file may contain any kind of ROOT objects, histograms,
# pictures, graphics objects, detector geometries, tracks, events, etc.
with TFile("py-hsimple.root", "RECREATE", "Demo ROOT file with histograms") as hfile:
    # Save all created objects in the file
    hfile.WriteObject(hpx)
    hfile.WriteObject(hpxpy)
    hfile.WriteObject(hprof)
    # TNTuple is special because it is a TTree-derived class. To make sure all the
    # dataset is properly written to disk, we connect it to the file and then
    # we ask the ntuple to write all the information to the file itself.
    ntuple.SetDirectory(hfile)
    ntuple.Write()

input("Press Enter to exit...")
