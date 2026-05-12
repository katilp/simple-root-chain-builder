#include <TChain.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <vector>
#include <iostream>

// Ensure ROOT types are available
#ifdef __CLING__
#include <TROOT.h>
#endif

void pfInvMassPlots(const char* filePattern = "*.root") {
    TChain *chain = new TChain("Events");
    
    // Add returns the number of files added. Store it directly.
    Int_t nFiles = chain->Add(filePattern);
    
    if (nFiles == 0) {
        std::cerr << "Error: No files found matching \"" << filePattern << "\"" << std::endl;
        return;
    }

    std::cout << "Loaded " << nFiles << " files. Total entries: " << chain->GetEntries() << std::endl;

    // 1. Simple Plots
    TCanvas *c1 = new TCanvas("c1", "nPFCands", 800, 400);
    chain->Draw("nPFCands");
    c1->SaveAs("h_num_cands.png");

    TCanvas *c2 = new TCanvas("c2", "PFCands_pdgId", 800, 400);
    chain->Draw("PFCands_pdgId");
    c2->SaveAs("h_pdgid_cands.png");

    // 2. Manual Loop Variables - EXACT TYPES FOR NANOAOO
    // Use UInt_t for counts, Int_t for IDs, Float_t for kinematics
    UInt_t nPFCands = 0;
    std::vector<Int_t> *PFCands_pdgId = nullptr;
    std::vector<Float_t> *PFCands_pt = nullptr;
    std::vector<Float_t> *PFCands_eta = nullptr;
    std::vector<Float_t> *PFCands_phi = nullptr;
    std::vector<Float_t> *PFCands_mass = nullptr;

    // Set Branch Addresses
    chain->SetBranchAddress("nPFCands", &nPFCands);
    chain->SetBranchAddress("PFCands_pdgId", &PFCands_pdgId);
    chain->SetBranchAddress("PFCands_pt", &PFCands_pt);
    chain->SetBranchAddress("PFCands_eta", &PFCands_eta);
    chain->SetBranchAddress("PFCands_phi", &PFCands_phi);
    chain->SetBranchAddress("PFCands_mass", &PFCands_mass);

    // Histograms for Masses
    TH1F *h_dimuon = new TH1F("h_dimuon", "Dimuon Mass;M (GeV);Events", 120, 0, 120);
    TH1F *h_dielectron = new TH1F("h_dielectron", "Dielectron Mass;M (GeV);Events", 120, 0, 120);

    Long64_t nEntries = chain->GetEntries();
    
    for (Long64_t i = 0; i < nEntries; i++) {
        chain->GetEntry(i);
        if (i % 100000 == 0) std::cout << "Entry " << i << std::endl;

        std::vector<Int_t> mu_idx;
        std::vector<Int_t> el_idx;

        // Loop over candidates
        for (UInt_t j = 0; j < nPFCands; ++j) {
            Int_t pdg = PFCands_pdgId->at(j);
            Float_t pt = PFCands_pt->at(j);

            if (pt < 2.0) continue;

            if (TMath::Abs(pdg) == 13) mu_idx.push_back(j);
            else if (TMath::Abs(pdg) == 11) el_idx.push_back(j);
        }

        // Dimuons
        for (size_t j = 0; j < mu_idx.size(); ++j) {
            for (size_t k = j + 1; k < mu_idx.size(); ++k) {
                Int_t i1 = mu_idx[j];
                Int_t i2 = mu_idx[k];
                if (PFCands_pdgId->at(i1) * PFCands_pdgId->at(i2) >= 0) continue;

                TLorentzVector p1, p2;
                p1.SetPtEtaPhiM(PFCands_pt->at(i1), PFCands_eta->at(i1), PFCands_phi->at(i1), PFCands_mass->at(i1));
                p2.SetPtEtaPhiM(PFCands_pt->at(i2), PFCands_eta->at(i2), PFCands_phi->at(i2), PFCands_mass->at(i2));
                h_dimuon->Fill((p1 + p2).M());
            }
        }

        // Dielectrons
        for (size_t j = 0; j < el_idx.size(); ++j) {
            for (size_t k = j + 1; k < el_idx.size(); ++k) {
                Int_t i1 = el_idx[j];
                Int_t i2 = el_idx[k];
                if (PFCands_pdgId->at(i1) * PFCands_pdgId->at(i2) >= 0) continue;

                TLorentzVector p1, p2;
                p1.SetPtEtaPhiM(PFCands_pt->at(i1), PFCands_eta->at(i1), PFCands_phi->at(i1), PFCands_mass->at(i1));
                p2.SetPtEtaPhiM(PFCands_pt->at(i2), PFCands_eta->at(i2), PFCands_phi->at(i2), PFCands_mass->at(i2));
                h_dielectron->Fill((p1 + p2).M());
            }
        }
    }

    // Save Mass Plots
    TCanvas *c3 = new TCanvas("c3", "Dimuon", 800, 600);
    h_dimuon->SetLineColor(kBlue);
    h_dimuon->Draw();
    c3->SaveAs("h_dimuon_mass.png");

    TCanvas *c4 = new TCanvas("c4", "Dielectron", 800, 600);
    h_dielectron->SetLineColor(kRed);
    h_dielectron->Draw();
    c4->SaveAs("h_dielectron_mass.png");

    std::cout << "Done." << std::endl;
}
