

void mergeResults()
{

    int n = 0;
    while (true) {
        std::ifstream file(Form("shieldingHeightResult%d.root", n));
        if (!file.good()) break;
        file.close();
        n++;
    }
    n--;


    if (n < 0) {
        std::cout << "No ROOT files found." << std::endl;
        return 1;
    }


    TChain chain("TreeOfData");


    for (int i = 0; i <= n; ++i) {
        chain.Add(Form("shieldingHeightResult%d.root", i));
    }


    TFile mergedFile("mergedResults.root", "RECREATE");


    TTree* mergedTree = chain.CloneTree();


    mergedTree->Write();
    mergedFile.Close();

    std::cout << "Merged " << n + 1 << " ROOT files." << std::endl;

    return 0;
}