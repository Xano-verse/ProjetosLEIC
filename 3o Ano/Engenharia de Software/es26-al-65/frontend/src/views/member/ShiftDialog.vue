<template>
  <v-dialog v-model="dialog" persistent width="1300">
    <v-card>
      <v-card-title>
        <span class="headline">
          {{
              'New Shift'
          }}
        </span>
      </v-card-title>
      <v-card-text>
        <v-form ref="form" lazy-validation>
          <v-row>
            <v-col>
              <VueCtkDateTimePicker
                id="startingDateInput"
                v-model="editShift.startTime"
                format="YYYY-MM-DDTHH:mm:ssZ"
                label="*Starting Date"
              ></VueCtkDateTimePicker>
            </v-col>
            <v-col>
              <VueCtkDateTimePicker
                id="endingDateInput"
                v-model="editShift.endTime"
                format="YYYY-MM-DDTHH:mm:ssZ"
                label="*Ending Date"
              ></VueCtkDateTimePicker>
            </v-col>
          </v-row>
          <v-row>
            <v-col cols="12">
              <v-text-field
                label="*Location"
                :rules= "[(v) => !!v || 'Location is required',
                          (v) => (v && v.length >= 20) || 'Minimum 20 characters',
                          (v) => (v && v.length <= 200) || 'Maximum 200 characters']"
                required
                v-model="editShift.location"
                data-cy="locationInput"
              ></v-text-field>
            </v-col>
            <v-col cols="12" sm="6" md="4">
              <v-text-field
                label="*Participants Limit"
                :rules="[
                  (v) =>
                    isNumberValid(v) ||
                    'Participants limit above 0 is required',
                ]"
                required
                v-model="editShift.participantsLimit"
                data-cy="participantsLimitInput"
              ></v-text-field>
            </v-col>
          </v-row>
        </v-form>
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn
          color="blue-darken-1"
          variant="text"
          @click="$emit('close-shift-dialog')"
        >
          Close
        </v-btn>
        <v-btn
          :disabled="!canSave"
          color="blue-darken-1"
          variant="text"
          @click="saveShift"
          data-cy="saveShift"
        >
          Save
        </v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>
<script lang="ts">
import { Vue, Component, Prop, Model } from 'vue-property-decorator';
import VueCtkDateTimePicker from 'vue-ctk-date-time-picker';
import 'vue-ctk-date-time-picker/dist/vue-ctk-date-time-picker.css';
import { ISOtoString } from '@/services/ConvertDateService';
import Shift from '@/models/shift/Shift';

Vue.component('VueCtkDateTimePicker', VueCtkDateTimePicker);
@Component({
  methods: { ISOtoString },
})
export default class ShiftDialog extends Vue {
  @Model('dialog', Boolean) dialog!: boolean;
  @Prop({ type: Shift, required: true }) readonly shift!: Shift;

  editShift: Shift = new Shift();

  async created() {
    // Local copy created in this function to avoid changing the Prop itself is safer
    this.editShift = new Shift(this.shift);
  }

  isNumberValid(value: any) {
    if (!/^\d+$/.test(value)) return false;
    const parsedValue = parseInt(value);
    return parsedValue > 0;
  }

  get canSave(): boolean {
    const l = this.editShift.location

    const isLocationValid = !!l && l.length >= 20 && l.length <= 200

    return (
      !!this.editShift.startTime &&
      !!this.editShift.endTime &&
      !!this.editShift.location &&
      isLocationValid &&
      !!this.editShift.participantsLimit
    );
  }

async saveShift() {
    if ((this.$refs.form as Vue & { validate: () => boolean }).validate()) {
      this.$emit('save-shift', this.editShift);
    }
}

}
</script>

<style scoped lang="scss"></style>
