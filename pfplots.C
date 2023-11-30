// Plots two variables from a PF collection and writes them out to png files
{
// // Declare a TCanvas
TCanvas *c1 = new TCanvas("c1", "Number of packed candidates", 800, 400);
TCanvas *c2 = new TCanvas("c2", "Type of packed candidates", 800, 400);
//
c1->cd(0);
Events->Draw("nPFCands");
c1->SaveAs("h_num_cands.png");
//
c2->cd(0);
Events->Draw("PFCands_pdgId");
c2->SaveAs("h_pdgid_cands.png");
//
}
