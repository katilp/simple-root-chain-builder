#include <TChain.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <vector>
#include <iostream>

void pfInvMassPlots(const char* filePattern = "*.root") {
    TChain chain("Events");
    Int_t nFiles = chain.Add(filePattern);
    
    if (nFiles == 0) {
        std::cerr << "Error: No files found matching \"" << filePattern << "\"" << std::endl;
        return;
    }
    std::cout << "Loaded " << nFiles << " files. Total entries: " << chain.GetEntries() << std::endl;

    // 1. Simple Plots (Direct Draw still works fine)
    TCanvas *c1 = new TCanvas("c1", "Number of packed candidates", 800, 400);
    chain.Draw("nPFCands");
    c1->SaveAs("h_num_cands.png");

    TCanvas *c2 = new TCanvas("c2", "Type of packed candidates", 800, 400);
    chain.Draw("PFCands_pdgId");
    c2->SaveAs("h_pdgid_cands.png");

    // 2. Setup TTreeReader (Type-Safe Alternative to SetBranchAddress)
    TTreeReader reader(&chain);
    
    // Define readers for the branches
    // TTreeReaderArray automatically handles the vector type matching
    TTreeReaderValue<UInt_t> nPFCands(reader, "nPFCands");
    TTreeReaderArray<Int_t>   PFCands_pdgId(reader, "PFCands_pdgId");
    TTreeReaderArray<Float_t> PFCands_pt(reader, "PFCands_pt");
    TTreeReaderArray<Float_t> PFCands_eta(reader, "PFCands_eta");
    TTreeReaderArray<Float_t> PFCands_phi(reader, "PFCands_phi");
    TTreeReaderArray<Float_t> PFCands_mass(reader, "PFCands_mass");

    // Histograms
    TH1F *h_dimuon = new TH1F("h_dimuon", "Dimuon Invariant Mass;M_{#mu#mu} (GeV);Events", 120, 0, 120);
    h_dimuon->SetLineColor(kBlue);
    h_dimuon->SetLineWidth(2);

    TH1F *h_dielectron = new TH1F("h_dielectron", "Dielectron Invariant Mass;M_{ee} (GeV);Events", 120, 0, 120);
    h_dielectron->SetLineColor(kRed);
    h_dielectron->SetLineWidth(2);

    Long64_t nEntries = chain.GetEntries();
    
    // 3. Loop using Reader
    while (reader.Next()) {
        // Access values directly using [] or .Get()
        UInt_t nCands = *nPFCands;
        
        std::vector<Int_t> mu_idx;
        std::vector<Int_t> el_idx;

        // Loop over candidates
        for (UInt_t j = 0; j < nCands; ++j) {
            Int_t pdg = PFCands_pdgId[j];
            Float_t pt = PFCands_pt[j];

            if (pt < 2.0) continue;

            if (TMath::Abs(pdg) == 13) mu_idx.push_back(j);
            else if (TMath::Abs(pdg) == 11) el_idx.push_back(j);
        }

        // Dimuons
        if (mu_idx.size() >= 2) {
            for (size_t j = 0; j < mu_idx.size(); ++j) {
                for (size_t k = j + 1; k < mu_idx.size(); ++k) {
                    Int_t i1 = mu_idx[j];
                    Int_t i2 = mu_idx[k];
                    
                    if (PFCands_pdgId[i1] * PFCands_pdgId[i2] >= 0) continue;

                    TLorentzVector p1, p2;
                    p1.SetPtEtaPhiM(PFCands_pt[i1], PFCands_eta[i1], PFCands_phi[i1], PFCands_mass[i1]);
                    p2.SetPtEtaPhiM(PFCands_pt[i2], PFCands_eta[i2], PFCands_phi[i2], PFCands_mass[i2]);
                    h_dimuon->Fill((p1 + p2).M());
                }
            }
        }

        // Dielectrons
        if (el_idx.size() >= 2) {
            for (size_t j = 0; j < el_idx.size(); ++j) {
                for (size_t k = j + 1; k < el_idx.size(); ++k) {
                    Int_t i1 = el_idx[j];
                    Int_t i2 = el_idx[k];
                    
                    if (PFCands_pdgId[i1] * PFCands_pdgId[i2] >= 0) continue;

                    TLorentzVector p1, p2;
                    p1.SetPtEtaPhiM(PFCands_pt[i1], PFCands_eta[i1], PFCands_phi[i1], PFCands_mass[i1]);
                    p2.SetPtEtaPhiM(PFCands_pt[i2], PFCands_eta[i2], PFCands_phi[i2], PFCands_mass[i2]);
                    h_dielectron->Fill((p1 + p2).M());
                }
            }
        }
    }

    // Save Mass Plots
    TCanvas *c3 = new TCanvas("c3", "Dimuon Mass", 800, 600);
    h_dimuon->Draw();
    c3->SaveAs("h_dimuon_mass.png");

    TCanvas *c4 = new TCanvas("c4", "Dielectron Mass", 800, 600);
    h_dielectron->Draw();
    c4->SaveAs("h_dielectron_mass.png");

    std::cout << "All plots saved successfully." << std::endl;
}
