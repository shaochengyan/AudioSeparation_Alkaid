import torch
from asteroid.models import ConvTasNet
import yaml

# get the convtasnet model with configuration in yaml file
def get_model(conf_filepath: str):
    with open("./conf/conf_model1.yml") as f:
        conf = yaml.safe_load(f)
    model = ConvTasNet(
        **conf["filterbank"], **conf["masknet"], sample_rate=16000
    )
    return model

if __name__ == '__main__':
    model1 = get_model("./conf/conf_model1.yml")
    # model2 = get_model("./conf/conf_model2.yml")
    
    # dummy input music
    dummy_input = torch.rand(size=(16000, ))
    dummy_output = model1(dummy_input)
    print("dummy_output.shape = ", dummy_input.shape)  # single channel music
    print("dummy_output.shape = ", dummy_output.shape)  # two source


