bool isSorted = true;
  for(int i = 0; i < finalRelevance.size()-j; i++)
  {//В случе одинаковой релевантности, сверяем у кого id меньше и меняем местами по необходимости
   if(finalRelevance[i].second < finalRelevance[i+1].second &&
   finalRelevance[i].first == finalRelevance[i+1].first)
   {
   pair<int, int> temp = {finalRelevance[i].first, finalRelevance[i].second};//Временная пара
   finalRelevance[i] = finalRelevance[i+1];
   finalRelevance[i+1] = temp;
   isSorted = false;
   }        
  }
  if (isSorted) 
  {