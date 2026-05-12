void pfplotschain(const char* filePattern = "*.root") {
    // Create the chain and add all files matching the pattern immediately
    TChain *chain = new TChain("Events");
    chain->Add(filePattern);

    // Check if files were actually loaded
    if (chain->GetEntries() == 0) {
        std::cout << "Error: No files found matching \"" << filePattern << "\"" << std::endl;
        return;
    }

    // Declare canvases
    TCanvas *c1 = new TCanvas("c1", "Number of packed candidates", 800, 400);
    TCanvas *c2 = new TCanvas("c2", "Type of packed candidates", 800, 400);

    // Plot 1
    c1->cd();
    chain->Draw("nPFCands");
    c1->SaveAs("h_num_cands.png");

    // Plot 2
    c2->cd();
    chain->Draw("PFCands_pdgId");
    c2->SaveAs("h_pdgid_cands.png");
}
