from tools.Updater.Updater import FireBase
from tools.fetchingData import Retriever
from tools.model import Models
import time


 
if __name__ == "__main__":
    firebase = FireBase()
    data = firebase.retrieve_data()
    latitude,longitude = data.get('lat'), data.get('long')
    models = Models()
    
    
    while True:
        retriever = Retriever(latitude,longitude)
        
        retriever.getResponses()
        
        data = firebase.retrieve_data()
        
        firebase.retrieve_image()
        
        
        retriever.average()
        retriever.currentData()
        
        #Updating with current data
        currentdata = {'Temperature':retriever.temperature,'Humidity':retriever.humidity,'Precipitation':retriever.precipitation,'SolarRadiation':retriever.solarRadiation}
        data.update(currentdata) #Updating firebase data with current weather data
        
        
        evoapotranspiration = retriever.averages[5]
        precipitation = retriever.averages[2]
        
        disease = models.predictDisease()
        #pest = "Healthy"
        pest = models.predictPest()
        
        data.update({"DiseaseStatus":disease,"PestsStatus":pest})
        
        
        fieldArea = data.get('fieldarea')
        
        
        #Effective precipitation. (Since not all water is absorbed by the plant due to rainfall condition)
        EP = precipitation * 0.8 
        
        
        requiredIrrigation = max(evoapotranspiration - EP, 0) #required irrigation per day
        
        waterAmountRequired = (requiredIrrigation * 0.001 * fieldArea) * 1000  #Calculation of water required in cubic meter
        
        
        
        evoapotranspirationAlert = diseaseAlert = pestsAlert = False
        
        if evoapotranspiration > 5.9:
             evoapotranspirationAlert = True
        
        if disease != "Healthy":
            diseaseAlert = True
            
        if pest != "Healthy":
            pestsAlert = True
        
        #Updating with additional data
        data.update({"AmountOfWater":waterAmountRequired, "evapotranspirationAlert":evoapotranspirationAlert, "pestsAlert":pestsAlert, "diseaseAlert":diseaseAlert})
        
        firebase.update_data(data)
    
        time.sleep(120)
