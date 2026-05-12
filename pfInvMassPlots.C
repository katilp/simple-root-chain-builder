void pfInvMassPlots(const char* filePattern = "*.root") {
    // 1. Setup the Chain
    TChain *chain = new TChain("Events");
    Int_t nFiles = chain->Add(filePattern);
    
    if (nFiles == 0) {
        std::cout << "Error: No files found matching \"" << filePattern << "\"" << std::endl;
        return;
    }
    std::cout << "Loaded " << nFiles << " files. Total entries: " << chain->GetEntries() << std::endl;

    // 2. Simple Plots via Direct Draw
    // These handle vector expansion and filling automatically.
    
    // Plot 1: nPFCands
    TCanvas *c1 = new TCanvas("c1", "Number of packed candidates", 800, 400);
    chain->Draw("nPFCands"); 
    c1->SaveAs("h_num_cands.png");

    // Plot 2: PFCands_pdgId
    TCanvas *c2 = new TCanvas("c2", "Type of packed candidates", 800, 400);
    chain->Draw("PFCands_pdgId");
    c2->SaveAs("h_pdgid_cands.png");

    // 3. Setup for Manual Loop (Invariant Mass)
    // Only load branches needed for the complex pairing logic
    int nPFCands = 0;
    std::vector<int> *PFCands_pdgId = 0;
    std::vector<float> *PFCands_pt = 0;
    std::vector<float> *PFCands_eta = 0;
    std::vector<float> *PFCands_phi = 0;
    std::vector<float> *PFCands_mass = 0;

    chain->SetBranchAddress("nPFCands", &nPFCands);
    chain->SetBranchAddress("PFCands_pdgId", &PFCands_pdgId);
    chain->SetBranchAddress("PFCands_pt", &PFCands_pt);
    chain->SetBranchAddress("PFCands_eta", &PFCands_eta);
    chain->SetBranchAddress("PFCands_phi", &PFCands_phi);
    chain->SetBranchAddress("PFCands_mass", &PFCands_mass);

    // Create Histograms for Invariant Masses
    TCanvas *c3 = new TCanvas("c3", "Dimuon Invariant Mass (PF)", 800, 600);
    TH1F *h_dimuon_mass = new TH1F("h_dimuon_mass", "Dimuon Invariant Mass (PF);M_{#mu#mu} (GeV);Events", 120, 0, 120);
    h_dimuon_mass->SetLineColor(kBlue);
    h_dimuon_mass->SetLineWidth(2);

    TCanvas *c4 = new TCanvas("c4", "Dielectron Invariant Mass (PF)", 800, 600);
    TH1F *h_dielectron_mass = new TH1F("h_dielectron_mass", "Dielectron Invariant Mass (PF);M_{ee} (GeV);Events", 120, 0, 120);
    h_dielectron_mass->SetLineColor(kRed);
    h_dielectron_mass->SetLineWidth(2);

    // 4. Manual Event Loop (Only for Complex Logic)
    Long64_t nEntries = chain->GetEntries();
    
    for (Long64_t i = 0; i < nEntries; i++) {
        chain->GetEntry(i);
        if (i % 50000 == 0) std::cout << "Processing entry " << i << "/" << nEntries << " (Mass calc)" << std::endl;

        // Temporary containers for indices
        std::vector<int> muonIndices;
        std::vector<int> electronIndices;

        if (PFCands_pdgId && PFCands_pt) {
            for (int j = 0; j < nPFCands; j++) {
                int pdg = PFCands_pdgId->at(j);
                float pt = PFCands_pt->at(j);

                // Minimal PT cut to reduce combinatorics
                if (pt < 2.0) continue; 

                if (abs(pdg) == 13) {
                    muonIndices.push_back(j);
                } else if (abs(pdg) == 11) {
                    electronIndices.push_back(j);
                }
            }
        }

        // Dimuon Pairing
        if (muonIndices.size() >= 2) {
            for (size_t j = 0; j < muonIndices.size(); j++) {
                for (size_t k = j + 1; k < muonIndices.size(); k++) {
                    int idx1 = muonIndices[j];
                    int idx2 = muonIndices[k];
                    if (PFCands_pdgId->at(idx1) * PFCands_pdgId->at(idx2) >= 0) continue; // Same charge skip

                    TLorentzVector p1, p2;
                    p1.SetPtEtaPhiM(PFCands_pt->at(idx1), PFCands_eta->at(idx1), PFCands_phi->at(idx1), PFCands_mass->at(idx1));
                    p2.SetPtEtaPhiM(PFCands_pt->at(idx2), PFCands_eta->at(idx2), PFCands_phi->at(idx2), PFCands_mass->at(idx2));
                    h_dimuon_mass->Fill((p1 + p2).M());
                }
            }
        }

        // Dielectron Pairing
        if (electronIndices.size() >= 2) {
            for (size_t j = 0; j < electronIndices.size(); j++) {
                for (size_t k = j + 1; k < electronIndices.size(); k++) {
                    int idx1 = electronIndices[j];
                    int idx2 = electronIndices[k];
                    if (PFCands_pdgId->at(idx1) * PFCands_pdgId->at(idx2) >= 0) continue; // Same charge skip

                    TLorentzVector p1, p2;
                    p1.SetPtEtaPhiM(PFCands_pt->at(idx1), PFCands_eta->at(idx1), PFCands_phi->at(idx1), PFCands_mass->at(idx1));
                    p2.SetPtEtaPhiM(PFCands_pt->at(idx2), PFCands_eta->at(idx2), PFCands_phi->at(idx2), PFCands_mass->at(idx2));
                    h_dielectron_mass->Fill((p1 + p2).M());
                }
            }
        }
    }

    // 5. Draw and Save Mass Plots
    c3->cd();
    h_dimuon_mass->Draw();
    c3->SaveAs("h_dimuon_mass.png");

    c4->cd();
    h_dielectron_mass->Draw();
    c4->SaveAs("h_dielectron_mass.png");

    std::cout << "All plots saved successfully." << std::endl;
}
