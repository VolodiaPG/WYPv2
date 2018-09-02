import { Component, OnInit } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { WebsocketWrapperService } from '../websocket-wrapper.service';
import { WebSocketModule, Commands_t } from '../websocket-module';
import { Observable } from 'rxjs';
import { SelectorFlags } from '@angular/core/src/render3/interfaces/projection';

@Component({
  selector: 'app-details',
  templateUrl: './details.component.html',
  styleUrls: ['./details.component.scss']
})
export class DetailsComponent implements OnInit {

  private websocket: WebsocketWrapperService;
  private route: ActivatedRoute;
  private nodeId: number;
  private messageSentFlag: boolean = false;
  public mod: WebSocketModule;

  public initialized: boolean = false;

  constructor(private _route: ActivatedRoute, private ws: WebsocketWrapperService) {
    this.websocket = ws;
    this.route = _route;
    this.route.params.subscribe(params => {
      this.nodeId = +params.nodeId;
      this.mod = this.GetModule();
    });

    var self = this;
    this.websocket.moduleAdded.subscribe($event => {
      self.mod = self.GetModule();
      self._Init();
    });
  }

  GetModule(): WebSocketModule {
    var ret = this.websocket.GetModule(this.nodeId);
    return ret;
  }

  ngOnInit() {
    this.initialized = false;
    var self = this;
    this.websocket.parameterReceived.subscribe($event => {
      self._OnRequestFullfill($event);
    })
    this._Init();
  }

  private _Init(): void {
    if (this.mod && !this.messageSentFlag) {
      this.mod.Request(Commands_t.time | Commands_t.uptime | Commands_t.timezone);
      this.messageSentFlag = true;
    }
  }

  SetTimezone(value: number): void {
    if (this.mod) {
      this.mod.SetTimezone(value);
    }
  }

  private _OnRequestFullfill(param: Object): void {
    if (param['module'] && param['module'] === this.mod) {
      if (param['commands'] & Commands_t.time && param['commands'] & Commands_t.uptime && param['commands'] & Commands_t.timezone) {
        this.initialized = true;
      }
    }
  }

}
